from django.urls import path

from .views import (
    RegisterAPIView,
    LoginAPIView,
    LogoutAPIView,
    ProfileAPIView,
    UserListAPIView,
    UserDetailAPIView,
)

app_name = "accounts_api"

urlpatterns = [

    # Authentication
    path(
        "register/",
        RegisterAPIView.as_view(),
        name="register",
    ),

    path(
        "login/",
        LoginAPIView.as_view(),
        name="login",
    ),

    path(
        "logout/",
        LogoutAPIView.as_view(),
        name="logout",
    ),

    # Profile
    path(
        "profile/",
        ProfileAPIView.as_view(),
        name="profile",
    ),

    # Users
    path(
        "users/",
        UserListAPIView.as_view(),
        name="user-list",
    ),

    path(
        "users/<int:pk>/",
        UserDetailAPIView.as_view(),
        name="user-detail",
    ),

]
