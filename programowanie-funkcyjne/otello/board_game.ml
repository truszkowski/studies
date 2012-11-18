(*** B O A R D  G A M E ***)

module BoardGame : BOARD_GAME =
	functor (B : BGAME_BOARD) ->
	functor (AI : BGAME_AI with 
		type t_field=B.t_field and
		type t_board=B.t_board and
		type t_move=B.t_move and
		type t_position=B.t_position) ->
	functor (GUI : BGAME_GUI with 
		type t_field=B.t_field and
		type t_board=B.t_board and
		type t_move=B.t_move and
		type t_position=B.t_position) ->
		struct

(* otwieramy moduly: *)
		open B
		open AI
		open GUI

(* pobranie zdarzenia (Graphics) *)
		let getEvent () =
			let e = Graphics.wait_next_event [Graphics.Button_down; Graphics.Key_pressed]
			in if e.Graphics.keypressed then Key e.Graphics.key
				else Mouse (e.Graphics.mouse_x,e.Graphics.mouse_y)

(* uruchomienie *)
		let game_init () = 
			Graphics.open_graph (" "^gui_resolutions);
			Graphics.set_window_title gui_title

(* wyjscie *)	
		let game_exit () = 
			Graphics.clear_graph ();
			Graphics.moveto ((Graphics.size_x ())/2 - 40 ) ((Graphics.size_y ())/2 - 5);
			Graphics.draw_string "Saving new weights...";
			writeAI ai_patterns ai_weights;
			Graphics.close_graph ();
			print_string ("Goodbye!\n")

(* kto z kim ma grac, lista graczy *)
		let rec game_properities n = 
			if n = 0 then []
				else (Graphics.clear_graph ();
					Graphics.moveto ((Graphics.size_x ())/2 - 80) ((Graphics.size_y ())/2 + 20);
					(if (n = 1)
						then Graphics.draw_string "Choose second player..."
						else Graphics.draw_string "Choose first player...");
					Graphics.moveto ((Graphics.size_x ())/2 - 80) ((Graphics.size_y ())/2 + 5);
					Graphics.draw_string "c - Computer, h - Human";
					Graphics.moveto ((Graphics.size_x ())/2 - 80) ((Graphics.size_y ())/2 - 10);
					Graphics.draw_string "e - Exit";
					Graphics.moveto 0 0;
					(match (getEvent ()) with
						Key 'h' -> Human::(game_properities (n - 1)) |
						Key 'c' -> Computer::(game_properities (n - 1)) |
						Key 'e' -> raise Event_Exit |
						_ -> game_properities n))

(* Tu mozna skladac gratulacje *)
		let game_gg result = 
			(fun x -> ((* press any key *))) (getEvent ())

(* pomocniczo... *)
		let square x = x *. x
		let repair x = 1. /. (1. +. (exp (-.x *. ai_scale_evaluate)))
		let repair_derive x = 
			ai_scale_evaluate *. 
			(exp (-.x *. ai_scale_evaluate)) /. 
			(ai_scale_evaluate *. (1. +. (exp (-.x *. ai_scale_evaluate))))
		let rec niter f n x = 
			if n = 0 then x else niter f (n - 1) (f x)

(* algorytm oceniania planszy na podstawie patternow *)

(* "niezreperowana" funkcja *)
		let evaluate_nscale board who =
			let boa = (if who then board else (invert board))
			in List.fold_right (fun p v -> (AI_Hash.find ai_patterns (p boa)) +. v) ai_features 0.0

(* "zreperowana" funkcja *)			
		let evaluate board who =
			if (isEndGame board) 
				then match (gameResult board) with 
					Win x -> if x = who then 1.0 else 0.0 |
					_ -> 0.5
				else repair (evaluate_nscale board who)

(* wybor maximum/minimum z listy par: (wart,plansza) *)
		let maxpair vallist =
			List.fold_right 
				(fun x y -> if (fst x) > (fst y) then x else y) 
				(List.tl vallist) (List.hd vallist)

(* poprawa wartosci wag *)
		let better board who oldvl newvl =
			let patts = List.map (fun p -> p (if who then board else (invert board))) ai_features
			in let f p = 
				AI_Hash.replace ai_patterns p 
					(AI_Hash.find ai_patterns p +. ((2.0 *. ai_scale_step *. (newvl -. (repair oldvl)) *. (repair_derive oldvl))))
			in List.iter f patts

(* alfa-beta obiecie... *)
		let alphabeta deep board alpha who = 
			let rec ab d board w = 
				if (isEndGame board) (* gdy koniec *)
					then (match gameResult board with 
						Win x -> if x = who then (1.0, pass board w) else (0.0, pass board w) |
						_ -> (0.5, pass board w))
				else let lg = legal board w
					in let leg = if lg = []
						then let mv = pass board w in [(evaluate mv who, pass mv w)]
						else List.map (fun el -> let mv = move board el w in (evaluate mv who, mv)) lg 
					in let choose l =
						let f next now = 
							if w = who 
								then if (fst next) >= alpha 
									then next::now else now
								else if (fst next) < (fst (List.hd now))
									then [next] else now
						in (List.fold_right f (List.tl l) [List.hd l])
					in let g el = ((fst (ab (d - 1) (snd el) (not w))), (snd el)) 
					in if d = 0 (* siegnelismy dna *)
						then maxpair leg
						else maxpair (List.map g (choose leg))
			in let result = (ab deep board who)
			in let boa = snd result
			in better boa who (evaluate_nscale boa who) (fst result); boa

(* najlepszy ruch *)
		let bestmove board who =
			let leg = legal board who
			in match (leg) with
				([]) -> pass board who |
				(mv::[]) -> move board mv who |
				_ -> alphabeta ai_deep_search board 0.4 who

(* zapis stanu gry *)
		let game_save board who = 
			gui_save board true;
			gui_gamelog (string_of_float (evaluate board true))

(* obslugiwanie turowej rozgrywki *)
		let rec game who player1 player2 board boardList =
			gui_show board who; game_save board who;
			if isEndGame board 
				then gameResult board (* koniec gry *)
				else if (who && (player1 = Computer)) || ((not who) && (player2 = Computer)) 
					then let newBoard = bestmove board who (* ruch komputera *)
						in game (not who) player1 player2 newBoard (newBoard::boardList)
					else (* ruch czlowieka *)
						try (match getEvent () with
							Key a -> let newBoard = gui_key board a who
								in game (not who) player1 player2 newBoard (newBoard::boardList) |
							Mouse (a,b) -> let newBoard = gui_mouse board (a,b) who
								in game (not who) player1 player2 newBoard (newBoard::boardList))
						with (* jesli nie dostalismy planszy to obslugujemy wyjatki *)
							Event_Empty -> game who player1 player2 board boardList |
							Event_Reset -> let sb = startBoard () in game true player1 player2 sb [sb] |
							Event_Back -> (match boardList with (* cofanie ruchu mojego i przeciwnika *)
								(recently1::recently2::recently3::recently4) -> 
									game who player1 player2 recently3 (recently3::recently4) |
								_ ->
									game who player1 player2 board boardList) |
							Event_Surrender -> Win (not who) 

(* Glówna petla gry *)
		let game_run () =
			try 
				game_init ();
				let rec game_next () = 
					let board = B.startBoard () 
					in let pl = game_properities 2
					in game_gg (game true (List.nth pl 0) (List.nth pl 1) board [board]); game_next () 
				in try game_next () 
					with Event_Exit -> game_exit ()
			with (* gdy wyjetek konczymy (np: Graphics.Graphic_failure) *)
				e -> game_exit () 

(*
 * minimalizujemy blad oceny sytuacji:
 * W pierwszym kroku zbieramy oceny ze wszystkich patternow ze wszystkich lekcji
 * Dalej zmieniamy wagi dotyczasowe uwzgledniajac zebrane oceny z lekcji.
 *)
		let minimize_error lessons hashtbl = 
			let len = float_of_int (List.length lessons)
			in let magic = 1.0
			in let funk les h = 
				let boa = fst les
				in let rv = snd les and ev = evaluate_nscale boa true
				in let lstp = List.map (fun p -> p boa) ai_features
				in let f x = 
					try let vl = AI_Hash.find h x 
						in AI_Hash.replace h x (vl +. (rv -. (repair ev)) *. (repair_derive ev))
					with Not_found -> AI_Hash.add h x ((rv -. (repair ev)) *. (repair_derive ev))
				in (List.iter f lstp; h)
			in let hlptbl = List.fold_right funk lessons (AI_Hash.create (AI_Hash.length ai_patterns))
			in let ff id vl = 
				try let ovl = AI_Hash.find hashtbl id
					in AI_Hash.replace hashtbl id (ovl +. (2.0 *. magic /. len *. vl)) 
				with Not_found -> ()
			in AI_Hash.iter ff hlptbl; hashtbl

(* Nauka na zestawach treningowych *)
		let lessons destination learn iter = 
			writeAI (niter (minimize_error (ai_loadLessons learn)) iter ai_patterns) destination
			
(* trening: komp - komp *)
		let training str it =
			let rec game board who = (* komp-komp *)
				if B.isEndGame board 
					then match gameResult board with 
						Win x -> if who = x then (1.0, 0.0) else (0.0, 1.0) |
						_ -> (0.5, 0.5)
					else game (bestmove board who) (not who)
			in let funk x = 
				let g = game (startBoard ()) true
				in ((fst g) +. (fst x), (snd g) +. (snd x))
			in let nit = (fun x -> writeAI ai_patterns str; (niter funk 10 x))
			in let (res1, res2) = niter nit (it/10 + 1) (0.0, 0.0)
			in (writeAI ai_patterns str; (* jeszcze tylko bilans gier... *)
				print_float res1; print_string " : "; print_float res2; print_newline ())

		end;;
