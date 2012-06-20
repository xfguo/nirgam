function plot_graphs(T)
load ../recvd_pkt;
CM = [0 0.55 1;1 0 0;0 0 1;0 1 0;1 0.55 0];
row=recvd_pkt(1,1);
col=recvd_pkt(1,2);
A=[];
k=2;
for i=1:row
    for j=1:col
        A(i,j)=recvd_pkt(k,2);
        k=k+1;
    end
end
R=cell(row);
C=cell(col);
for i=0:row-1
    R{i+1}=int2str(i);
end
for i=0:col-1
    C{i+1}=int2str(i);
end
CM=hot(16);
%load latency_flit
%figure(1);
h = bar3(A);
view([-37.5 30]);
 set(gca,'PlotBoxAspectRatio',[3.8 5 3.09]);
set(gca,'fontweight','bold');
set(gca,'YTickLabel',R);
set(gca,'XTickLabel',C);
title('Traffic Distribution','fontsize',12,'fontweight','bold');
xlabel('Routers along Y-direction','fontsize',12,'fontweight','bold','Rotation',20);
ylabel('Routers along X-direction','fontsize',12,'fontweight','bold','Rotation',-37);
zlabel('No. of Packets Received','fontsize',12,'fontweight','bold');
colormap jet;
k=0;

for i = 1:length(h)
    zdata = ones(6*length(h),4);
    k = 1;
    for j = 0:6:(6*length(h)-6)
        zdata(j+1:j+6,:) = A(k,i);
        k = k+1;
    end
    set(h(i),'Cdata',zdata)
end
% shading interp
% for i = 1:length(h)
%     zdata = get(h(i),'Zdata');
%     set(h(i),'Cdata',zdata)
%     set(h,'EdgeColor','k')
% end


%colorbar
k=0;
for i = 1:size(A,2)
	for j = 1:size(A,1)
		str = num2str(k);
		lbl = strcat('R',str);
		text(i,j,0,lbl,'fontweight','bold');
		k = k + 1;
	end
end
