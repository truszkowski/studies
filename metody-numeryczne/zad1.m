% badana funkcja 
function ret = f(x)
%	ret = 3*x + x^(1/2) - 3;
	ret = ((x^(2) + 2)^(1/2) + x - 1)/10;
endfunction

% pochodna badanej funkcji
function ret = g(x)
%	ret = 3 + x^(-1/2)/2;
	ret = (x*(x^(2) + 2)^(-1/2) + 1)/10;
endfunction

% ponizej 4 rozne metody iteracyjne, kazda zwraca wektor 
% wynikow w kazdej iteracji.

function ret = newton(x, v)
	ret = (1 : v)';
	ret(1) = x;
	i = 2;
	it = rows(ret);
	while ( i <= it )
		ret(i) = ret(i-1) - f( ret(i-1) ) / g( ret(i-1) );
		++i;
	endwhile
endfunction

function ret = sieczne(x0, x1, v)
	ret = (1 : v)';
	ret(1) = x0;
	ret(2) = x1;
	i = 3;
	it = rows(ret);
	while ( i <= it )
		ret(i) = ret(i-1) - f( ret(i-1) ) * ( ret(i-1) - ret(i-2) ) / ( f( ret(i-1) ) - f( ret(i-2) ) );
		++i;
	endwhile
endfunction

function ret = bisekcja(a, b, v) % zalozenie:  a < 0 < b
	ret = (1 : v)';
	ret(1) = (a + b)/2;
	i = 2;
	it = rows(ret);
	while ( i <= it )	
		if ( f( ret(i-1) ) > 0 ) 
			b = ret(i-1);
		else
			a = ret(i-1);
		endif
		ret(i) = (a + b)/2;
		++i;
	endwhile
endfunction

function ret = steffensen(x, v) 
	ret = (1 : v)';
	ret(1) = x;
	i = 2;
	it = rows(ret);
	while ( i <= it )
		ret(i) = ret(i-1) - ((f(ret(i-1)))^(2)) / (f(ret(i-1) + f(ret(i-1))) - f(ret(i-1)));
		++i;
	endwhile
endfunction

% prezentacja
function zad1(iter)
	x = ( 1 : iter )';
	it = 10;
	x0 = 3;
	x1 = 2.9;
	xx = -0.5;
	ynewton = newton(x0, iter);
	ysieczne = sieczne(x0, x1, iter);
	ybisekcja = bisekcja(-x1, x1, iter);
	ysteffensen	= steffensen(x1, iter);
	ylabel("wartość x w kolejnych iteracjach");
	xlabel("liczba iteracji");
	plot(x,ynewton,"-;metoda Newtona;");
	hold on;
	plot(x,ysieczne,"-;metoda siecznych;");
	plot(x,ybisekcja,"-;metoda bisekcji;");
	plot(x,ysteffensen,"-;metoda Steffensena;");
	hold off;
	figure
	ylabel("wartość błędu w kolejnych iteracjach");
	xlabel("liczba iteracji");
	plot(x,ynewton - xx*ones(iter,1),"-;metoda Newtona;");
	hold on;
	plot(x,ysieczne - xx*ones(iter,1),"-;metoda siecznych;");
	plot(x,ybisekcja - xx*ones(iter,1),"-;metoda bisekcji;");
	plot(x,ysteffensen - xx*ones(iter,1),"-;metoda Steffensena;");
	hold off;
endfunction

% wykres funkcji f
function rysuj_f(a,b)
	x = ( a : 0.01 : b )';
	y = x;
	i = 1;
	len = rows(x);
	while ( i <= len )
		y(i) = f(x(i));
		++i;
	endwhile
	axis("auto");
	plot(x,y,";f(x);");
endfunction
