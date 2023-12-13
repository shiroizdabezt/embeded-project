from rest_framework import serializers
from .models import dht22

class ItemSerializer(serializers.ModelSerializer):
    class Meta:
        model = dht22
        fields = "__all__"