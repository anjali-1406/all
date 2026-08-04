from django.core.cache import cache
from django.db.models import Prefetch

from django_filters.rest_framework import DjangoFilterBackend

from rest_framework import permissions, status
from rest_framework.decorators import action
from rest_framework.filters import OrderingFilter, SearchFilter
from rest_framework.pagination import PageNumberPagination
from rest_framework.response import Response
from rest_framework.viewsets import ModelViewSet

from apps.comments.models import Comment
from apps.notification.models import Notification
from apps.posts.models import Post

from .permissions import IsOwnerOrReadOnly
from .serializers import PostSerializer


class PostPagination(PageNumberPagination):
    page_size = 10
    page_size_query_param = "page_size"
    max_page_size = 100


class PostViewSet(ModelViewSet):

    serializer_class = PostSerializer
    permission_classes = [permissions.IsAuthenticated, IsOwnerOrReadOnly]
    pagination_class = PostPagination

    filter_backends = [
        DjangoFilterBackend,
        SearchFilter,
        OrderingFilter,
    ]

    filterset_fields = ["author"]
    search_fields = ["content", "author__username"]
    ordering_fields = ["created_at", "updated_at"]
    ordering = ["-created_at"]

    def get_queryset(self):
        return (
            Post.objects.select_related("author")
            .prefetch_related(
                "likes",
                Prefetch(
                    "comments",
                    queryset=Comment.objects.select_related("author"),
                ),
            )
            .all()
        )

    # -------------------------
    # CACHE POSTS LIST
    # -------------------------
    def list(self, request, *args, **kwargs):

        cache_key = f"posts_{request.get_full_path()}"

        cached_data = cache.get(cache_key)

        if cached_data:
            return Response(cached_data)

        response = super().list(request, *args, **kwargs)

        cache.set(
            cache_key,
            response.data,
            timeout=300,   # 5 minutes
        )

        return response

    # -------------------------
    # CREATE POST
    # -------------------------
    def perform_create(self, serializer):

        serializer.save(author=self.request.user)

        cache.clear()

    # -------------------------
    # UPDATE POST
    # -------------------------
    def perform_update(self, serializer):

        serializer.save()

        cache.clear()

    # -------------------------
    # DELETE POST
    # -------------------------
    def perform_destroy(self, instance):

        Notification.objects.filter(
            post_id=instance.id
        ).delete()

        instance.delete()

        cache.clear()

    # -------------------------
    # LIKE / UNLIKE
    # -------------------------
    @action(detail=True, methods=["post"])
    def like(self, request, pk=None):

        post = self.get_object()

        if post.likes.filter(id=request.user.id).exists():

            post.likes.remove(request.user)

            Notification.objects.filter(
                sender=request.user,
                receiver=post.author,
                post_id=post.id,
                notification_type=Notification.LIKE,
            ).delete()

            cache.clear()

            return Response(
                {
                    "message": "Post unliked.",
                    "liked": False,
                    "total_likes": post.likes.count(),
                },
                status=status.HTTP_200_OK,
            )

        post.likes.add(request.user)

        if post.author != request.user:

            Notification.objects.get_or_create(
                sender=request.user,
                receiver=post.author,
                post_id=post.id,
                notification_type=Notification.LIKE,
            )

        cache.clear()

        return Response(
            {
                "message": "Post liked.",
                "liked": True,
                "total_likes": post.likes.count(),
            },
            status=status.HTTP_200_OK,
        )

    # -------------------------
    # MY POSTS
    # -------------------------
    @action(detail=False, methods=["get"])
    def my_posts(self, request):

        queryset = self.get_queryset().filter(
            author=request.user
        )

        page = self.paginate_queryset(queryset)

        if page is not None:

            serializer = self.get_serializer(
                page,
                many=True,
            )

            return self.get_paginated_response(
                serializer.data
            )

        serializer = self.get_serializer(
            queryset,
            many=True,
        )

        return Response(serializer.data)

    # -------------------------
    # LIKED POSTS
    # -------------------------
    @action(detail=False, methods=["get"])
    def liked_posts(self, request):

        queryset = self.get_queryset().filter(
            likes=request.user
        )

        page = self.paginate_queryset(queryset)

        if page is not None:

            serializer = self.get_serializer(
                page,
                many=True,
            )

            return self.get_paginated_response(
                serializer.data
            )

        serializer = self.get_serializer(
            queryset,
            many=True,
        )

        return Response(serializer.data)
