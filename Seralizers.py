from datetime import date
import re

from django.contrib.auth import get_user_model

from rest_framework import serializers

User = get_user_model()


# ==========================
# User Serializer
# ==========================

class UserSerializer(serializers.ModelSerializer):

    class Meta:
        model = User
        fields = [
            "id",
            "username",
            "email",
            "first_name",
            "last_name",
            "birth_date",
            "gender",
        ]
        read_only_fields = ["id"]


# ==========================
# Register Serializer
# ==========================

class RegisterSerializer(serializers.ModelSerializer):

    password = serializers.CharField(
        write_only=True
    )

    confirm_password = serializers.CharField(
        write_only=True
    )

    class Meta:
        model = User

        fields = [
            "username",
            "first_name",
            "last_name",
            "email",
            "password",
            "confirm_password",
            "birth_date",
            "gender",
        ]

    # -----------------------
    # Email Validation
    # -----------------------

    def validate_email(self, value):

        if User.objects.filter(email__iexact=value).exists():
            raise serializers.ValidationError(
                "Email already exists."
            )

        return value

    # -----------------------
    # Password Validation
    # -----------------------

    def validate_password(self, value):

        if len(value) < 8:
            raise serializers.ValidationError(
                "Password must be at least 8 characters."
            )

        if not re.search(r"[A-Z]", value):
            raise serializers.ValidationError(
                "Password must contain one uppercase letter."
            )

        if not re.search(r"[a-z]", value):
            raise serializers.ValidationError(
                "Password must contain one lowercase letter."
            )

        if not re.search(r"\d", value):
            raise serializers.ValidationError(
                "Password must contain one number."
            )

        if not re.search(r"[!@#$%^&*(),.?\":{}|<>]", value):
            raise serializers.ValidationError(
                "Password must contain one special character."
            )

        return value

    # -----------------------
    # Birth Date Validation
    # -----------------------

    def validate_birth_date(self, value):

        today = date.today()

        age = (
            today.year
            - value.year
            - (
                (today.month, today.day)
                <
                (value.month, value.day)
            )
        )

        if age < 18:
            raise serializers.ValidationError(
                "You must be at least 18 years old."
            )

        return value

    # -----------------------
    # Object Validation
    # -----------------------

    def validate(self, attrs):

        if attrs["password"] != attrs["confirm_password"]:
            raise serializers.ValidationError(
                {
                    "confirm_password":
                    "Passwords do not match."
                }
            )

        return attrs

    # -----------------------
    # Create User
    # -----------------------

    def create(self, validated_data):

        validated_data.pop("confirm_password")

        password = validated_data.pop("password")

        user = User.objects.create_user(
            password=password,
            **validated_data
        )

        return user


# ==========================
# Login Serializer
# ==========================

class LoginSerializer(serializers.Serializer):

    email = serializers.EmailField()

    password = serializers.CharField(
        write_only=True
    )

    def validate(self, attrs):

        email = attrs.get("email")
        password = attrs.get("password")

        try:
            user = User.objects.get(
                email__iexact=email
            )

        except User.DoesNotExist:

            raise serializers.ValidationError(
                "Invalid email or password."
            )

        if not user.check_password(password):

            raise serializers.ValidationError(
                "Invalid email or password."
            )

        if not user.is_active:

            raise serializers.ValidationError(
                "User account is inactive."
            )

        attrs["user"] = user

        return attrs
