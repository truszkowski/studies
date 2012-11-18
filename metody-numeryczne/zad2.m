global ITER = 1000000;
global EPSILON = 0.000000000001;
global TIME = 300;

function Jacobi(alfa, N) 
global ITER;
global EPSILON;
global TIME;
	x = rand(1,N);                       % dokladnie tyle wynosi x
	pom1 = circshift(x,[0,1]);
	pom2 = circshift(x,[0,-1]);
	pom1(1) = pom2(N) = 0;
	b = alfa*x - pom1 - pom2;            % tyle wynosi b
	lambda = (alfa + 2*cos(pi/(N+1)));
	eta = alfa - lambda;
	iter = 0;
	y = zeros(1,N);                      % a tu bedziemy probowali zbiegac do x
	tic();
	while ( ++iter ) 
		pom1 = circshift(y,[0,1]);
		pom2 = circshift(y,[0,-1]);
		pom1(1) = pom2(N) = 0;
		y = -(eta*y - pom1 - pom2 - b)/lambda;
		if ( norm(x-y) <= EPSILON )
			break; % wystarczajaco dokladnie
		elseif ( iter > ITER ) 
			break; % no niestety za duzo iteracji
		elseif ( toc() > TIME )
			break; % za dlugo, ile mozna czekac
		endif
	endwhile
	% wyniki...
	fprintf(" Metoda Jocobiego (alfa: %f, N: %d)\n", alfa, N);
	fprintf("   Iteracji: %d\n", iter);
	fprintf("    Czas: %fsek\n", toc());
	fprintf("    Blad: %e\n", norm(x - y)); 
endfunction

function Richardson(alfa, N) 
global ITER;
global EPSILON;
global TIME;
	x = rand(1,N);                       % dokladnie tyle wynosi x
	pom1 = circshift(x,[0,1]);
	pom2 = circshift(x,[0,-1]);
	pom1(1) = pom2(N) = 0;
	b = alfa*x - pom1 - pom2;            % tyle wynosi b
	iter = 0;
	y = zeros(1,N);                      % a tu bedziemy probowali zbiegac do x
	tic();
	while ( ++iter ) 
		pom1 = circshift(y,[0,1]);
		pom2 = circshift(y,[0,-1]);
		pom1(1) = pom2(N) = 0;
		y = y - (alfa*y - pom1 - pom2 - b) / alfa;
		if ( norm(x-y) <= EPSILON )
			break; % wystarczajaco dokladnie
		elseif ( iter > ITER ) 
			break; % no niestety za duzo iteracji
		elseif ( toc() > TIME )
			break; % za dlugo, ile mozna czekac
		endif
	endwhile
	% wyniki...
	fprintf(" Metoda Richardsona (alfa: %f, N: %d)\n", alfa, N);
	fprintf("    Iteracji: %d\n", iter);
	fprintf("    Czas: %fsek\n", toc());
	fprintf("    Blad: %e\n", norm(x - y)); 
endfunction

function CG(alfa, N) 
global ITER;
global EPSILON;
global TIME;
	x = rand(1,N);                       % dokladnie tyle wynosi x
	pom1 = circshift(x,[0,1]);
	pom2 = circshift(x,[0,-1]);
	pom1(1) = pom2(N) = 0;
	b = alfa*x - pom1 - pom2;            % tyle wynosi b
	iter = 0;
	y = zeros(1,N);                      % a tu bedziemy probowali zbiegac do x
	
	res = b;
	ro = norm(res)^2;
	betha = 0; k = 1;
	p = zeros(1,N);
	
	tic();
	while ( ++iter ) 
		p = res + betha * p;
		
		pom1 = circshift(p,[0,1]);
		pom2 = circshift(p,[0,-1]);
		pom1(1) = pom2(N) = 0;
		w = alfa*p - pom1 - pom2;
	
		alpha = ro / ( p * w' );
		y = y + alpha * p;
		res = res - alpha * w;
		
		ron = ro;
		ro = norm(res)^2;
		betha = ro / ron;
		++k;
	
		if ( norm(x-y) <= EPSILON )
			break; % wystarczajaco dokladnie
		elseif ( iter > ITER ) 
			break; % no niestety za duzo iteracji
		elseif ( toc() > TIME )
			break; % za dlugo, ile mozna czekac
		endif
	endwhile
	% wyniki...
	fprintf(" Metoda CG (alfa: %f, N: %d)\n", alfa, N);
	fprintf("    Iteracji: %d\n", iter);
	fprintf("    Czas: %fsek\n", toc());
	fprintf("    Blad: %e\n", norm(x - y)); 
endfunction
