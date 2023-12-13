from django.db import models

# Create your models here.
class dht22(models.Model):
    temperature = models.FloatField()
    humidity = models.FloatField()

    uvLevel = models.FloatField()

    timestamp = models.DateTimeField(auto_now_add=True)
        
    device_name = models.CharField(max_length=255, null=True)
    ipaddr = models.CharField(max_length=255, null=True)



    def __str__(self):
        #return f"temperature: {self.temperature}, humidity: {self.humidity},timestamp: {self.timestamp}"
        return self.timestamp