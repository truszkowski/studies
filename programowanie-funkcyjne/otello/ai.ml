(*** A I ***)

module OthelloAI (B : BGAME_BOARD) =
	struct

		open B
		type t_field = B.t_field
		type t_board = B.t_board
		type t_move = B.t_move
		type t_position = B.t_position
		type t_feature = int * t_field list
		
(* Wlasny sposob haszowania *)  
		module AI_HashType =
			struct	 
				type t = t_feature 
				let equal = (=)
				let hash id = 
					let l = (snd id)
					in (fst id) + (10 * (List.length l - 4)) +
						(100 * (List.fold_right (fun el res -> (3 * res) + el + 1) l 0))
			end;;
		
		module AI_Hash = Hashtbl.Make (AI_HashType)
		
(* parametry... *)
		let ai_weights = "othello.weights" 
		let ai_scale_evaluate = 1.0 /. 21.0
		let ai_scale_step = 0.017
		let ai_deep_search = 4
		let ai_alphabetaCut = 0.3

(***** P A T T E R N Y *****)

(* rozszerzenie List.hd (wyjmuje n elementow) *)	
		let rec myHd l n = 
			match n with
				0 -> [] |
				_ -> (List.hd l)::(myHd (List.tl l) (n - 1))

(* rozszerzenie List.tl (ucina n elementow) *)
		let rec myTl l n = 
			match n with
				0 -> l |
				_ -> myTl (List.tl l) (n - 1)
		
(* pomocnik dla diagonalnych patternow *)
		let rec diag ill start left =
			if (left && (start = 0)) || ((not left) && (start = 9)) || (ill = [])
				then []
				else (List.nth (List.hd ill) (start - 1))::(diag (List.tl ill) (start + (if left then -1 else 1)) left)

(***** U K O S N E *****)
				
		let pattern_diag4_1 board =  
			(0, diag board.b 4 true)
		
		let pattern_diag4_2 board =  
			(0, diag board.b 5 false)

		let pattern_diag4_3 board =  
			let l = myTl board.b 4
			in (0, diag l 1 false)
		
		let pattern_diag4_4 board =  
			let l = myTl board.b 4
			in (0, diag l 8 true)
		
		let pattern_diag5_1 board =  
			(1, diag board.b 5 true)
		
		let pattern_diag5_2 board =  
			(1, diag board.b 4 false)
		
		let pattern_diag5_3 board =  
			let l = myTl board.b 3
			in (1, diag l 1 false)
		
		let pattern_diag5_4 board =
			let l = myTl board.b 3
			in (1, diag l 8 true)
		
		let pattern_diag6_1 board =  
			(2, diag board.b 6 true)
		
		let pattern_diag6_2 board =  
			(2, diag board.b 3 false)
		
		let pattern_diag6_3 board =  
			let l = myTl board.b 2
			in (2, diag l 1 false)
		
		let pattern_diag6_4 board =  
			let l = myTl board.b 2
			in (2, diag l 8 true)
		
		let pattern_diag7_1 board =  
			(3, diag board.b 7 true)

		let pattern_diag7_2 board =  
			(3, diag board.b 2 false)

		let pattern_diag7_3 board =  
			let l = myTl board.b 1
			in (3, diag l 1 false)

		let pattern_diag7_4 board =  
			let l = myTl board.b 1
			in (3, diag l 8 true)
		
		let pattern_diag8_1 board =  
			(4, diag board.b 8 true)

		let pattern_diag8_2 board =  
			(4, diag board.b 1 false)
		
(***** P I O N O W E  I  P O Z I O M E *****)
		
		let pattern_hv2_1 board =  
			(5, List.hd (List.tl board.b))

		let pattern_hv2_2 board =  
			(5, List.hd (myTl board.b 6))
		
		let pattern_hv2_3 board =  
			(5, (List.map (fun l -> List.nth l 1) board.b))
		
		let pattern_hv2_4 board =  
			(5, (List.map (fun l -> List.nth l 6) board.b))
		
		let pattern_hv3_1 board =  
			(6, List.hd (myTl board.b 2))
		
		let pattern_hv3_2 board =  
			(6, List.hd (myTl board.b 5))
		
		let pattern_hv3_3 board =  
			(6, (List.map (fun l -> List.nth l 2) board.b))
		
		let pattern_hv3_4 board =  
			(6, (List.map (fun l -> List.nth l 5) board.b))
		
		let pattern_hv4_1 board =  
			(7, List.hd (myTl board.b 3))
		
		let pattern_hv4_2 board =  
			(7, List.hd (myTl board.b 4))
		
		let pattern_hv4_3 board =  
			(7, (List.map (fun l -> List.nth l 3) board.b))
		
		let pattern_hv4_4 board =  
			(7, (List.map (fun l -> List.nth l 4) board.b))
		
(***** B R Z E G O W E *****)
		
		let pattern_edge_1 board = 
			let l = (List.hd (List.tl board.b))
			in (8, (List.hd board.b) @ [(List.nth l 1);(List.nth l 6)])
		
		let pattern_edge_2 board = 
			let lp = (myTl board.b 6) in let l = (List.hd lp)
			in (8, (List.hd (List.tl lp)) @ [(List.nth l 1);(List.nth l 6)])
		
		let pattern_edge_3 board = 
			let lp = (myTl board.b 1) 
			in let l1 = List.hd lp in let l2 = List.hd (myTl lp 5)
			in (8, (List.map (fun l -> List.nth l 7) board.b) @ [(List.nth l1 6);(List.nth l2 6)])
		
		let pattern_edge_4 board = 
			let lp = (myTl board.b 1) 
			in let l1 = List.hd lp in let l2 = List.hd (myTl lp 5)
			in (8, (List.map (fun l -> List.nth l 0) board.b) @ [(List.nth l1 1);(List.nth l2 1)])
			
(***** R O G I  -  2 x 5 *****)
		
		let pattern_2x5_1 board = 
			let l1 = List.hd board.b in let l2 = List.hd (List.tl board.b)
			in (9, (myHd l1 5) @ (myHd l2 5))
			
		let pattern_2x5_2 board = 
			let l1 = (List.hd board.b) in let l2 = (List.hd (List.tl board.b))
			in (9, (List.rev ((myTl l2 3) @ (myTl l1 3))))
			
		let pattern_2x5_3 board = 
			let lp = myTl board.b 6
			in let l1 = (List.hd lp) in let l2 = (List.hd (List.tl lp))
			in (9, (myHd l2 5) @ (myHd l1 5))
		
		let pattern_2x5_4 board = 
			let lp = myTl board.b 6
			in let l1 = (List.hd lp) in let l2 = (List.hd (List.tl lp))
			in (9, (List.rev ((myTl l1 3) @ (myTl l2 3))))
		
		let pattern_2x5_5 board = 
			let lp = myHd board.b 5
			in (9, (List.map (fun l -> List.nth l 0) lp) @ (List.map (fun l -> List.nth l 1) lp))
		
		let pattern_2x5_6 board = 
			let lp = myTl board.b 3
			in (9, (List.rev ((List.map (fun l -> List.nth l 1) lp) @ (List.map (fun l -> List.nth l 0) lp))))
		
		let pattern_2x5_7 board = 
			let lp = myHd board.b 5 in let lpp = List.map (fun l -> myTl l 6) lp
			in (9, (List.map (fun l -> List.nth l 1) lpp) @ (List.map (fun l -> List.nth l 0) lpp))
		
		let pattern_2x5_8 board = 
			let lp = myTl board.b 3 in let lpp = List.map (fun l -> myTl l 6) lp
			in (9, (List.rev ((List.map (fun l -> List.nth l 0) lpp) @ (List.map (fun l -> List.nth l 1) lpp))))
			
(***** R O G I  -  3 x 3 *****)
		
		let pattern_3x3_1 board =
			let l = (List.map (fun lp -> myHd lp 3) (myHd board.b 3))
			in (10, List.fold_right (@) l [])
		
		let pattern_3x3_2 board =
			let l = (List.map (fun lp -> myTl lp 5) (myHd board.b 3))
			in (10, List.rev (List.fold_right (fun l1 l2 -> l2 @ l1) l []))
		
		let pattern_3x3_3 board =
			let l = (List.map (fun lp -> myHd lp 3) (myTl board.b 5))
			in (10, List.fold_right (fun l1 l2 -> l2 @ l1) l [])
		
		let pattern_3x3_4 board =
			let l = (List.map (fun lp -> myTl lp 5) (myTl board.b 5))
			in (10, List.rev (List.fold_right (@) l []))
		
		let pattern_count = 167265
		
(***** K O N I E C  P A T T E R N O W *****)

(* lista funkcji skladajaca sie na charakterystyke planszy *)
		let ai_features = 
			[pattern_diag4_1; pattern_diag4_2; pattern_diag4_3; pattern_diag4_4; (* 3^4      *)
			pattern_diag5_1; pattern_diag5_2; pattern_diag5_3; pattern_diag5_4;   (* 3^5      *)
			pattern_diag6_1; pattern_diag6_2; pattern_diag6_3; pattern_diag6_4;   (* 3^6      *)
			pattern_diag7_1; pattern_diag7_2; pattern_diag7_3; pattern_diag7_4;   (* 3^7      *)
			pattern_diag8_1; pattern_diag8_2;                                     (* 3^8      *)
			pattern_hv2_1; pattern_hv2_2; pattern_hv2_3; pattern_hv2_4;           (* 3^8      *)
			pattern_hv3_1; pattern_hv3_2; pattern_hv3_3; pattern_hv3_4;           (* 3^8      *)
			pattern_hv4_1; pattern_hv4_2; pattern_hv4_3; pattern_hv4_4;           (* 3^8      *)
			pattern_edge_1; pattern_edge_2; pattern_edge_3; pattern_edge_4;       (* 3^10     *)
			pattern_2x5_1; pattern_2x5_2; pattern_2x5_3; pattern_2x5_4;
			pattern_2x5_5; pattern_2x5_6; pattern_2x5_7; pattern_2x5_8;           (* 3^10     *)
			pattern_3x3_1; pattern_3x3_2; pattern_3x3_3; pattern_3x3_4]           (* 3^9      *)
			                                                                      (* = 167265 *)

(* wygenerowanie wszystkich mozliwosci charakterystyk na podsawie 
   patternow i wrzucenie go do tablicy haszujacej *)
		let generatePossibilities hashtbl = 
			let rec poss res id n vl len f =
				if n = 0 then (AI_Hash.replace hashtbl (id, res) (f res))
					else (
						poss ((1)::res) id (n - 1) (vl +. 1.) (len +. 1.) f;
						poss ((-1)::res) id (n - 1) (vl -. 1.) (len +. 1.) f;
						poss ((0)::res) id (n - 1) vl (len +. 1.) f)
			in let possibilities id n f = poss [] id n 0. 0. f
			in let fhelp1 n1 n2 r = (* dla 3x3 i 2x5 *)
				match (List.nth r n1, List.nth r n2) with 
					(1,_) -> 2. | (-1,_) -> -.2. | 
					(0,1) -> -.1. | (0,-1) -> 1. |
					_ -> 0.
			in let fhelp2 n1 n2 n3 n4 r = (* dla diag_8 i edge *)
				match (List.nth r n1, List.nth r n2, List.nth r n3, List.nth r n4) with
					(1,1,_,_) -> 3.6 | (-1,-1,_,_) -> -.3.6 | 
					(1,0,_,1) -> 0.5 | (0,1,1,_) -> 0.5 |
					(1,0,_,-1) -> 3. | (0,1,-1,_) -> 3. |
					(1,0,_,_) -> 2. | (0,1,_,_) -> 2. |
					(-1,0,_,-1) -> -.0.5 | (0,-1,-1,_) -> -.0.5 | 
					(-1,0,_,1) -> -.3. | (0,-1,1,_) -> -.3. | 
					(-1,0,_,_) -> -.2. | (0,-1,_,_) -> -.2. | 
					(0,0,1,0) -> -.1. | (0,0,0,1) -> -.1. |
					(0,0,-1,0) -> 1. | (0,0,0,-1) -> 1. |
					(0,0,1,1) -> -.2. | (0,0,-1,-1) -> 2. |
					_ -> 0.
			in begin (* dla wszystkich patternów mozna zdefiniowac ocene dla konkretnych przypadków *)
				possibilities 10 9 (fhelp1 0 4);
				possibilities 9 10 (fhelp1 0 6);
				possibilities 8 10 (fhelp2 0 7 8 9);
				possibilities 7 8 (fun r -> 0.);
				possibilities 6 8 (fun r -> 0.);
				possibilities 5 8 (fun r -> 0.);
				possibilities 4 8 (fhelp2 0 7 1 6);
				possibilities 3 7 (fun r -> 0.);
				possibilities 2 6 (fun r -> 0.);
				possibilities 1 5 (fun r -> 0.);
				possibilities 0 4 (fun r -> 0.);
			end

(* domyslna (startowa) tablica haszujaca *)
	let defaultAI () =
		let hashtbl = AI_Hash.create pattern_count
			in generatePossibilities hashtbl; hashtbl
						
(* odczyt tablicy haszujacej z pliku *)
		let readAI str = 
			let fd = open_in str
			in let hash = AI_Hash.create pattern_count
			in let rec record str = 
				let idstr = int_of_string (String.sub str 0 2)
				in let rec lst l s = 
					let st = (String.sub s 0 1) 
					in if st = "=" then ((idstr, (List.rev l)), float_of_string (String.sub s 1 ((String.length s) - 1)))
						else if st = "-" then lst ((-1)::l) ((String.sub s) 2 (String.length s - 2))
							else if st = "1" then lst (1::l) ((String.sub s) 1 (String.length s - 1))
								else lst (0::l) (String.sub s 1 ((String.length s) - 1))
				in lst [] (String.sub str 2 ((String.length str) - 2))
			in let rec loadall n =
				if n = 0 then (close_in fd; hash)
					else let rc = record (input_line fd)
					in (AI_Hash.add hash (fst rc) (snd rc)); loadall (n - 1)
			in loadall pattern_count

(* zapis tabllicy haszujacej do pliku *)
		let writeAI hashtbl str = 
			let fd = open_out str
			in let rec print_flds l = 
				if l = [] then "="
					else (string_of_int (List.hd l))^(print_flds (List.tl l))
			in let print id vl =
				let fstid = (fst id) 
				in let st = 
					(if fstid = 10 then string_of_int 10 else "0"^(string_of_int fstid))^
					(print_flds (snd id))^(string_of_float vl)^"\n"
				in output fd st 0 (String.length st)
			in AI_Hash.iter print hashtbl; close_out fd

(* tablica haszujaca *)
		let ai_patterns = 
			if Sys.file_exists ai_weights then readAI ai_weights
				else defaultAI ()

(* wczyt lekcji *)
		let ai_loadLessons from = 
			let fd = open_in from
				in let rec getOne lst = (* wyciagamy po jednej *)
					try 
						let rec boarding flds m = 
							if m = 0 then ({b= flds; c= 0; fp= 0; sp= 0}, (float_of_string (input_line fd)))
								else boarding ((let rec rows cols str n = 
										if n = 8 then cols
											else rows ((match (String.sub str n 1) with
													"X" -> (1) | "O" -> (-1) | _ -> (0)
												)::cols) str (n + 1)
									in rows [] (input_line fd) 0)::flds) (m - 1) 
						in getOne ((boarding [] 8)::lst)
					with End_of_file -> close_in fd; lst
				in getOne []

	end;;
