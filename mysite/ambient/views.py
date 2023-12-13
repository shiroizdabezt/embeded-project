from django.shortcuts import render
from rest_framework.decorators import api_view
from .models import dht22
from .serializers import ItemSerializer
from rest_framework.response import Response
import pytz
from django.http import JsonResponse
from django.views.decorators.http import require_GET

#Create your views here.
@api_view(['POST'])
def addData(request):
    serializer = ItemSerializer(data = request.data)
    if serializer.is_valid():
        serializer.save()
        return Response(serializer.data)
    else:
        return Response("Ló Chim bùm")



def line_chart(request):
    # data = dht22.objects.annotate(date=TruncSecond('timestamp')).values('date').annotate(temperature=F('temperature'), humidity=F('humidity')).order_by('date')
    # for entry in data:
    #     entry['date'] = entry['date'].astimezone(pytz.timezone('Asia/Ho_Chi_Minh'))
    # timestamps = [entry['date'].strftime('%Y-%m-%dT%H:%M:%S.%fZ') for entry in data]
    # #timestamps = [entry['date'].astimezone(timezone('Asia/Ho_Chi_Minh')) for entry in data]
    # temperatures = [entry['temperature'] for entry in data]
    # humidities = [entry['humidity'] for entry in data]
    # , {'timestamps': timestamps, 'temperatures': temperatures, 'humidities': humidities}

    devices = [data for data in dht22.objects.values('device_name', 'ipaddr').distinct()]

    return render(request, 'line_chart.html', {'devices': devices})

@require_GET
def data(request):
    ip = request.GET.get('ipaddr', None)
    all_sensor_data = dht22.objects.filter(ipaddr=ip).order_by('-timestamp')[:2]


    # Lấy giá trị cụ thể từ dữ liệu
    timestamps = [data.timestamp.astimezone(pytz.timezone('Asia/Ho_Chi_Minh')) for data in all_sensor_data]
    temperatures = [data.temperature for data in all_sensor_data]
    humidities = [data.humidity for data in all_sensor_data]

    uvLevel = [data.uvLevel for data in all_sensor_data]

    #devices_name = list(set([data.device_name for data in all_sensor_data]))
    ipaddrs = [data.ipaddr for data in all_sensor_data]


    # Bây giờ bạn có thể sử dụng các giá trị này trong logic của mình
    # Ví dụ: trả về chúng dưới dạng JSON  'devices_name': devices_name,
    return JsonResponse({'timestamps': timestamps.pop(1), 'temperatures': temperatures, 'humidities': humidities, 'uvLevel': uvLevel, 'ipaddrs': ipaddrs})