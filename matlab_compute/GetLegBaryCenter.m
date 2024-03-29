function [ml, Il, L, Lw, Lb] = GetLegBaryCenter(angle1, angle4, pitch)

angle1 = angle1 / 180 * pi;
angle4 = angle4 / 180 * pi;

l1 = 0.15;
l2 = 0.27;
l3 = 0.27;
l4 = 0.15;
l5 = 0.15;

ml1 = 0.1399;
ml2 = 0.14175;
ml3 = 0.1779;
ml4 = 0.10615;

Il1 = 0.00057637;
Il2 = 0.00161803;
Il3 = 0.00204813;
Il4 = 0.00047237;

scale1 = 0.683717133;
scale2 = 0.44485837;
scale3 = 0.362538407;
scale4 = 0.582931467;

ml = ml1 + ml2 + ml3 + ml4;

xa = -l5 / 2;
ya = 0;
xe = l5 / 2;
ye = 0;
xb = l1 * cos(angle1) - l5/2;
yb = l1 * sin(angle1);
xd = l5/2 + l4 * cos(angle4);
yd = l4 * sin(angle4);
lbd = sqrt((xd - xb).^2 + (yd - yb).^2);
A0 = 2 * l2 * (xd - xb);
B0 = 2 * l2 * (yd - yb);
C0 = l2.^2 + lbd.^2 - l3.^2;
D0 = l3.^2 + lbd.^2 - l2.^2;
u2 = 2 * atan((B0 + sqrt(A0.^2 + B0.^2 - C0.^2))/(A0 + C0));
u3 = pi - 2 * atan((-B0 + sqrt(A0.^2 + B0.^2 - D0.^2))/(A0 + D0));
xc = xb + l2 * cos(u2);
yc = yb + l2 * sin(u2);
R = [cos(pitch), -sin(pitch);
    sin(pitch), cos(pitch)];
v = R*[xc;yc];
xc = v(1);
yc = v(2);
L = sqrt(xc ^ 2 + yc ^ 2);

mx_l1 = scale1*(xb-xa)+xa;
my_l1 = scale1*(yb-ya)+ya;
mx_l2 = scale2*(xc-xb)+xb;
my_l2 = scale2*(yc-yb)+yb;
mx_l3 = scale3*(xc-xd)+xd;
my_l3 = scale3*(yc-yd)+yd;
mx_l4 = scale4*(xd-xe)+xe;
my_l4 = scale4*(yd-ye)+ye;

x = (mx_l1*ml1 + mx_l2*ml2 + mx_l3*ml3 + mx_l4*ml4)/(ml1+ml2+ml3+ml4);
y = (my_l1*ml1 + my_l2*ml2 + my_l3*ml3 + my_l4*ml4)/(ml1+ml2+ml3+ml4);

Lw = sqrt((xc - x)^2 + (yc - y)^2);
Lb = sqrt((xc)^2 + (yc)^2);
Lsum = Lw + Lb;

Lw = Lw / Lsum * L;
Lb = Lb / Lsum * L;

x = Lb / L * xc;
y = Lb / L * yc;

d1 = sqrt((x - mx_l1)^2 + (y - my_l1)^2);
d2 = sqrt((x - mx_l2)^2 + (y - my_l2)^2);
d3 = sqrt((x - mx_l3)^2 + (y - my_l3)^2);
d4 = sqrt((x - mx_l4)^2 + (y - my_l4)^2);

Il = (Il1 + ml1 *d1^2) + (Il2 + ml2 * d2^2) + (Il3 + ml3 * d3^2) + (Il4 + ml4 * d4^2);%平行轴定理
end