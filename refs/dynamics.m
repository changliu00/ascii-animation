function dynamics(len, S0, V0, a, b)
% larger a: smoother oscillation, and slower oscillation (stretched peaks)
% larger b: stronger oscillation (larger amptitude)
% S0: the start position of the curve. Smaller positive S0 will induce a sharper and higher peak for the 1/S curve at the beginning.
% V0: Larger (compared to 0) positive value will induce a lower peak of 1/S curve at beginning but will induce several severe peaks after the beginning.
	linewidth = 5;
	Sarr = zeros(1,len);
	Varr = zeros(1,len);
	S = S0; V = V0;
	Sarr(1) = S; Varr(1) = V;
	for i = 2:len
		V += (1-S)/a-V/b;
		S += V;
		Sarr(i) = S; Varr(i) = V;
	end
	% figure; plot(1:len, Sarr, 'linewidth', linewidth);
	% figure; plot(1:len, Varr, 'linewidth', linewidth);
	figure; plot(1:len, 1./Sarr, 'linewidth', linewidth);
	ylim([0, 10])

