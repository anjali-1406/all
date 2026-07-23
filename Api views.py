from django.contrib.auth import login, logout

from rest_framework import generics
from rest_framework import permissions
from rest_framework import status
from rest_framework.response import Response
from rest_framework.views import APIView

from django.contrib.auth import get_user_model

from .serializers import (
    UserSerializer,
    RegisterSerializer,
    LoginSerializer,
)

User = get_user_model()


# ==========================================
# Register API
# ==========================================

class RegisterAPIView(generics.CreateAPIView):

    queryset = User.objects.all()

    serializer_class = RegisterSerializer

    permission_classes = [
        permissions.AllowAny
    ]


# ==========================================
# Login API
# ==========================================

class LoginAPIView(APIView):

    permission_classes = [
        permissions.AllowAny
    ]

    def post(self, request):

        serializer = LoginSerializer(
            data=request.data
        )

        serializer.is_valid(
            raise_exception=True
        )

        user = serializer.validated_data["user"]

        login(request, user)

        return Response(
            {
                "message": "Login successful.",
                "user": UserSerializer(user).data,
            },
            status=status.HTTP_200_OK,
        )


# ==========================================
# Logout API
# ==========================================

class LogoutAPIView(APIView):

    permission_classes = [
        permissions.IsAuthenticated
    ]

    def post(self, request):

        logout(request)

        return Response(
            {
                "message": "Logout successful."
            },
            status=status.HTTP_200_OK,
        )


# ==========================================
# Logged In User Profile
# ==========================================

class ProfileAPIView(generics.RetrieveUpdateAPIView):

    serializer_class = UserSerializer

    permission_classes = [
        permissions.IsAuthenticated
    ]

    def get_object(self):

        return self.request.user


# ==========================================
# Users List
# ==========================================

class UserListAPIView(generics.ListAPIView):

    queryset = User.objects.all().order_by("id")

    serializer_class = UserSerializer

    permission_classes = [
        permissions.IsAuthenticated
    ]


# ==========================================
# User Detail
# ==========================================

class UserDetailAPIView(generics.RetrieveAPIView):

    queryset = User.objects.all()

    serializer_class = UserSerializer

    permission_classes = [
        permissions.IsAuthenticated
    ]
