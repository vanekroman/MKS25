clear all; close all;

data = csvread('ntc.csv');    % or use readmatrix('ntc.csv') in newer MATLAB
t = data(:,1);                % temperature (°C)
r = data(:,2);                % resistance (kΩ)

MAX = 1023;

R5 = 10;                          % 10 kΩ resistor
ad = MAX * r ./ (R5 + r);       % ADC values (NTC divider)

% Plot measured points
plot(ad, t, 'b');
xlabel('ADC value');
ylabel('Temperature (°C)');
grid on;
hold on;

% Polynomial fit and smoothed data
p = polyfit(ad, t, 10);

ad2 = 0:MAX;
t2 = round(polyval(p, ad2), 1);
plot(ad2, t2, 'r');
legend('Measured', 'Fitted');

% Save lookup table for C code (×10 for tenths of °C)
dlmwrite('data.dlm', t2 * 10, ',');








