mat = xlsread('rssi_values.xlsx')
rssiMat = mat(:,1:4);
clusters = mat(:,5);
mdl = fitcknn(rssiMat,clusters,'NumNeighbors',3,'Standardize',1);

current = [15,20,1,9] %  average of several signals
Location_label=predict(mdl,current)


echotcpip('on',8080) % test server running on port 8080

obj=tcpip('localhost',8080,'InputBufferSize',8000)
fopen(obj)
pause(1)
while(obj.BytesAvailable)
    data = fscanf(obj)
end
    
%{
FloorPlan = imread('FloorPlan.jpg');
imshow(FloorPlan);
hold on;
%}






%mdl = fitcknn(X,y,Name,Value)
% Location_label=predict(md1,current)

%{ 
predict
 load
 fscanf()
 plot(),

Creating a TCP/IP connection in MATLAB
 obj=tcpip('localhost',8080,'InputBufferSize',8000) %where
 8080 is the port number
 fopen(obj)
 pause(10)
 while(obj.BytesAvailable)
 data=fscanf(obj)

Drawing a circle on top of the image at coordinate x,y
 FloorPlan=imread(?.\FloorPlan.jpg');
 imshow(FloorPlan);
 hold on;
 plot(x,y,'r*');
 ang=0:0.01:2*pi;
xp=100*cos(ang);
yp=100*sin(ang);
plot(x+xp,y+yp);
 drawnow;
 hold off;

%}