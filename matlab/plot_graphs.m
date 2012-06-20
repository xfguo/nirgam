function plot_graphs()

C = [1 1 1;1 0 0;0 0 1;0 1 0;1 0.55 0];

load ../log/matlab/latency_flit;
load ../log/matlab/latency_pkt;
load ../log/matlab/throughput;
load ../log/matlab/power_tile;
figure(3);
h=bar3(power_tile);
set(gca,'fontweight','bold');
title('Power ','fontsize',12,'fontweight','bold');
xlabel('Routers along Y-direction','fontsize',12,'fontweight','bold','Rotation',20);
ylabel('Routers along X-direction','fontsize',12,'fontweight','bold','Rotation',-37);
zlabel('Power in watts','fontsize',12,'fontweight','bold');
colormap jet;
k=0;

for i = 1:length(h)
    zdata = ones(6*length(h),4);
    k = 1;
    for j = 0:6:(6*length(h)-6)
        zdata(j+1:j+6,:) = power_tile(k,i);
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


colorbar


%load latency_flit
figure(4);
h = bar3(latency_flit);
set(gca,'fontweight','bold');
title('Average latency per flit','fontsize',12,'fontweight','bold');
xlabel('Routers along Y-direction','fontsize',12,'fontweight','bold');
ylabel('Routers along X-direction','fontsize',12,'fontweight','bold');
zlabel('Average latency per flit (clock cycles)','fontsize',12,'fontweight','bold');
k = 0;
for i = 1:3:size(latency_flit,2)
	for j = 1:3:size(latency_flit,1)
		str = num2str(k);
		lbl = strcat('R',str);
		text(i,j-0.5,0,lbl,'fontweight','bold');
		k = k + 1;
	end
end

for i = 1:size(latency_flit,2)
    zdata = ones(6*size(latency_flit,1),3);
    k = 6*size(latency_flit,1)-6;
    if(rem(i,3) == 0)
        for j = 0:18:(6*size(latency_flit,1)-12)
            zdata(j+1:j+6,:) = 5;
            zdata(j+7:j+12,:) = 1;
            zdata(j+13:j+18,:) = 1;
        end
        zdata(k+1:k+6,:) = 5;
    elseif(rem(i+1,3) == 0)
        for j = 0:18:(6*size(latency_flit,1)-12)
            zdata(j+1:j+6,:) = 4;
            zdata(j+7:j+12,:) = 1;
            zdata(j+13:j+18,:) = 1;
        end
        zdata(k+1:k+6,:) = 4;
    else
         for j = 0:18:(6*size(latency_flit,1)-12)
            zdata(j+1:j+6,:) = 1;
            zdata(j+7:j+12,:) = 2;
            zdata(j+13:j+18,:) = 3;
        end
        zdata(k+1:k+6,:) = 1;
    end

    set(h(i),'Cdata',zdata)
end

colormap(C);

figure(5);
h = bar3(latency_pkt);
set(gca,'fontweight','bold');
title('Average latency per packet','fontsize',12,'fontweight','bold');
xlabel('Routers along Y-direction','fontsize',12,'fontweight','bold');
ylabel('Routers along X-direction','fontsize',12,'fontweight','bold');
zlabel('Average latency per packet (clock cycles)','fontsize',12,'fontweight','bold');
k = 0;
for i = 1:3:size(latency_pkt,2)
	for j = 1:3:size(latency_pkt,1)
		str = num2str(k);
		lbl = strcat('R',str);
		text(i,j-0.5,0,lbl,'fontweight','bold');
		k = k + 1;
	end
end

for i = 1:size(latency_pkt,2)
    zdata = ones(6*size(latency_pkt,1),3);
    k = 6*size(latency_pkt,1)-6;
    if(rem(i,3) == 0)
        for j = 0:18:(6*size(latency_pkt,1)-12)
            zdata(j+1:j+6,:) = 5;
            zdata(j+7:j+12,:) = 1;
            zdata(j+13:j+18,:) = 1;
        end
        zdata(k+1:k+6,:) = 5;
    elseif(rem(i+1,3) == 0)
        for j = 0:18:(6*size(latency_pkt,1)-12)
            zdata(j+1:j+6,:) = 4;
            zdata(j+7:j+12,:) = 1;
            zdata(j+13:j+18,:) = 1;
        end
        zdata(k+1:k+6,:) = 4;
    else
         for j = 0:18:(6*size(latency_pkt,1)-12)
            zdata(j+1:j+6,:) = 1;
            zdata(j+7:j+12,:) = 2;
            zdata(j+13:j+18,:) = 3;
        end
        zdata(k+1:k+6,:) = 1;
    end

    set(h(i),'Cdata',zdata)
end

colormap(C);

figure(6);
h = bar3(throughput);
set(gca,'fontweight','bold');
title('Average throughput','fontsize',12,'fontweight','bold');
xlabel('Routers along Y-direction','fontsize',12,'fontweight','bold');
ylabel('Routers along X-direction','fontsize',12,'fontweight','bold');
zlabel('Average throughput (Gbps)','fontsize',12,'fontweight','bold');
k = 0;
for i = 1:3:size(throughput,2)
	for j = 1:3:size(throughput,1)
		str = num2str(k);
		lbl = strcat('R',str);
		text(i,j-0.5,0,lbl,'fontweight','bold');
		k = k + 1;
	end
end

for i = 1:size(throughput,2)
    zdata = ones(6*size(throughput,1),3);
    k = 6*size(throughput,1)-6;
    if(rem(i,3) == 0)
        for j = 0:18:(6*size(throughput,1)-12)
            zdata(j+1:j+6,:) = 5;
            zdata(j+7:j+12,:) = 1;
            zdata(j+13:j+18,:) = 1;
        end
        zdata(k+1:k+6,:) = 5;
    elseif(rem(i+1,3) == 0)
        for j = 0:18:(6*size(throughput,1)-12)
            zdata(j+1:j+6,:) = 4;
            zdata(j+7:j+12,:) = 1;
            zdata(j+13:j+18,:) = 1;
        end
        zdata(k+1:k+6,:) = 4;
    else
         for j = 0:18:(6*size(throughput,1)-12)
            zdata(j+1:j+6,:) = 1;
            zdata(j+7:j+12,:) = 2;
            zdata(j+13:j+18,:) = 3;
        end
        zdata(k+1:k+6,:) = 1;
    end

    set(h(i),'Cdata',zdata)
end

colormap(C);

