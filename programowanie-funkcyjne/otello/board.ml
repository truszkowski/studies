(*** B O A R D  ***)

module OthelloBoard : BGAME_BOARD = 
	struct

		type t_field = int
		type t_board = { b : t_field list list ; c: int ; fp : int ; sp : int }
		type t_move = { mx : int ; my : int }
		type t_position = { px : int ; py : int }

(* plansza startowa *)
		let startBoard () = 
			{ b = 
			[[0; 0; 0; 0; 0; 0; 0; 0];
			 [0; 0; 0; 0; 0; 0; 0; 0];
			 [0; 0; 0; 0; 0; 0; 0; 0];
			 [0; 0; 0; -1; 1; 0; 0; 0];
			 [0; 0; 0; 1; -1; 0; 0; 0];
			 [0; 0; 0; 0; 0; 0; 0; 0];
			 [0; 0; 0; 0; 0; 0; 0; 0];
			 [0; 0; 0; 0; 0; 0; 0; 0]];
			  c = 0; fp = 2; sp = 2}

(* funkcja pomocnicza zmieniajaca bool (true,false) na odpowiadajace graczowi pole int (1,-1) *)
		let who2fld who =
			if who then 1 else -1

(* konwersja ruchu na pozycje *)
		let movetopos mov = 
			{ px = mov.mx ; py = mov.my }

(* iterator gdzie f ma dodatkowo za argumenty pozycje pol na planszy *)
		let iterator f board = 
			let rec cols f col x y =
				(match col with
					[] -> () |
					c::ol -> f {px= x; py= y} c; cols f ol (x + 1) y)
			in let rec rows f row x y =
				(match row with
					[] -> () |
					r::ow -> cols f r x y; rows f ow x (y - 1))
			in rows f board.b 1 8 

(* pole pod wskazana pozycja *)
		let position board pos = 
			let rec fhelp row n =
				if (n = pos.py)
					then let rec ffhelp col m =
						if (m = pos.px) then (List.hd col)
							else (ffhelp (List.tl col) (m + 1))
						in (ffhelp (List.hd row) 1)
					else (fhelp (List.tl row) (n - 1))
			in (fhelp board.b 8)

(* ustawia kamien gracza 'who' pod wskazana pozycje *)
		let setpos board pos who =
			let rec fhelp row n =
				if n = pos.py
					then let rec ffhelp col m =
						if m = pos.px
							then ((who2fld who)::(List.tl col))
							else ((List.hd col)::(ffhelp (List.tl col) (m + 1)))
						in ((ffhelp (List.hd row) 1)::(List.tl row))
					else ((List.hd row)::(fhelp (List.tl row) (n - 1)))
			in {b= (fhelp board.b 8); c= board.c; fp= board.fp; sp= board.sp}
	
(* pozycje sasiadow *)		
		let neighbours pos =
			match (pos.px, pos.py) with
				(1,1) -> 
					[{px=1; py=2}; {px=2; py=2}; 
					               {px=2; py=1}] |
				(1,8) -> 
					[              {px=2; py=8}; 
					 {px=1; py=7}; {px=2; py=7}] |
				(8,1) -> 
					[{px=7; py=2}; {px=8; py=2};
					 {px=7; py=1}] |
				(8,8) -> 
					[{px=7; py=8}; 
			     {px=7; py=7}; {px=8; py=7}] |
				(1,_) -> 
					[{px=1; py=(pos.py+1)}; {px=2; py=(pos.py+1)};
					                        {px=2; py=pos.py}; 
					 {px=1; py=(pos.py-1)}; {px=2; py=(pos.py-1)}] |
				(8,_) -> 
					[{px=7; py=(pos.py+1)}; {px=8; py=(pos.py+1)};
					 {px=7; py=pos.py};
					 {px=7; py=(pos.py-1)}; {px=8; py=(pos.py-1)}] |
				(_,1) -> 
					[{px=(pos.px-1); py=2}; {px=pos.px; py=2}; {px=(pos.px+1); py=2};
					 {px=(pos.px-1); py=1};                    {px=(pos.px+1); py=1}] |
				(_,8) -> 
					[{px=(pos.px-1); py=8};                    {px=(pos.px+1); py=8};
					 {px=(pos.px-1); py=7}; {px=pos.px; py=7}; {px=(pos.px+1); py=7}] |
				(_,_) -> 
					[{px=(pos.px-1); py=(pos.py+1)}; {px=pos.px; py=(pos.py+1)}; {px=(pos.px+1); py=(pos.py+1)};
					 {px=(pos.px-1); py=pos.py};                                 {px=(pos.px+1); py=pos.py};
					 {px=(pos.px-1); py=(pos.py-1)}; {px=pos.px; py=(pos.py-1)}; {px=(pos.px+1); py=(pos.py-1)}]
		
(* nastepne pole we wskazanym kierunku *)
		let nextstep pos1 pos2 = 
			let h = { px = pos2.px - pos1.px; py = pos2.py - pos1.py }
			in let pos = { px = pos2.px + h.px; py = pos2.py + h.py }
			in if ((pos.px > 8) || (pos.px < 1) || (pos.py > 8) || (pos.py < 1))
				then raise Out_Of_Range
				else pos

(* 
 * wykonanie ruchu - ,,niskopoziomowe'' bez sprawdzania legalnosci 
 * to inne ,,wyzsze'' funkcje korzystajace z move jak np gui_mouse 
 * dbaja o poprawnosc ruchu.
 *)
		let move board mov who = 
			let pos = movetopos mov 
			in let boa = setpos board pos who
			in let newBoard = 
				if who then {b= boa.b; c= (boa.c + 1); fp= (boa.fp+1); sp= boa.sp} 
					else {b= boa.b; c= (boa.c + 1); fp= boa.fp; sp= (boa.sp+1)} 
			in let neigh = neighbours pos
			in let rec check b pos1 pos2 = (* czy w tym kierunku rewersowac kamienie *)
				match ((who2fld who) * (position b pos2)) with (* jak who false plansze widzimy przeciwne wartosci *)
					-1 -> (try check b pos2 (nextstep pos1 pos2) 
						with Out_Of_Range -> false) |
					1 -> true |
					_ -> false
			in let rec modify b pos1 pos2 = (* rewersowanie kamieni *)
				match ((who2fld who) * (position b pos2)) with
					-1 -> let m = modify (setpos b pos2 who) pos2 (nextstep pos1 pos2) 
						in if who then {b= m.b; c= 0; fp= (m.fp+1); sp= (m.sp-1)}
							else {b= m.b; c= 0; fp= (m.fp-1); sp= (m.sp+1)} |
					_ -> b
			in let funk b n =
				if (check b pos n) then modify b pos n
					else b
			in List.fold_left funk newBoard neigh

		let pass board who =
			{b= board.b; c= (board.c+1); fp= board.fp; sp= board.sp}

(* mozliwe pozycje dla pola *)
		let legalpos board pos who =
			let rec findmove ppos npos =
				match ((who2fld who) * (position board npos)) with
					-1 -> (try findmove npos (nextstep ppos npos) 
						with Out_Of_Range -> []) |
					1 -> [] |
					_ -> [{mx = npos.px ; my = npos.py}]
			in List.fold_left (@) [] 
				(List.map 
					(function npos -> 
						(if (position board npos) = ((who2fld who) * -1)
							then (try findmove npos (nextstep pos npos) with Out_Of_Range -> [])
							else []))
					(neighbours pos))

(* mozliwe pozycje dla pól *)
		let legal board who =
			let rec iter x y cols rows =
				match (cols, rows) with
					([], []) -> [] |
					([], row::orow) -> (iter 1 (y - 1) row orow) |
					(col::ocol, _) -> if col = (who2fld who)
						then (legalpos board {px = x; py = y} who) @ (iter (x + 1) y ocol rows)
						else iter (x + 1) y ocol rows
			in iter 1 8 (List.hd board.b) (List.tl board.b)

(* czy ruch jest legalny *)
		let isLegal board mov who =
			let lb = legal board who
			in let rec find l =
				match l with
					[] -> false |
					l1::l2 -> if (l1.mx = mov.mx) && (l1.my = mov.my) 
						then true else find l2
			in if lb = [] 
				then true else find lb
		
(* czy koniec gry (brak wolnych pol, pasy obu graczy, jeden z graczy stracil swojej kamienie) *)
		let isEndGame board =
			(((board.fp + board.sp) = 64) || (board.c = 2) || (board.fp = 0) || (board.sp = 0))

(* wynik gry *)
		let gameResult board =
			if (board.fp > board.sp)
				then Win true
				else if (board.fp < board.sp)
					then Win false else Remis

(* zamiana graczy *)
		let invert board = 
			{b= List.map (fun row -> List.map (fun col -> -col) row) board.b;
			 c= board.c; fp= board.sp; sp= board.fp}

	end;;
