(*** G U I ***)

module OthelloGUI (B : BGAME_BOARD) =
	struct

		open B
		type t_field = B.t_field
		type t_board = B.t_board
		type t_move = B.t_move
		type t_position = B.t_position

(* parametry: *)
		let gui_resolutions = 
			if Sys.os_type = "Win32" then "410x430" else "400x400"
		let gui_title = "::. Othello .::"
		let weight = 40 
		let height = 40
		let gs_x = 0
		let gs_y = 0
		
(* wczytanie bitmapy z pliku *)
		let loadImage file_name =
			let c = ref 0 in let d = ref 54 in let bin_file = open_in_bin file_name
			in let bmp_arr = Array.create (in_channel_length bin_file + 1) 0
			in try while true do begin
						bmp_arr.(!c) <- input_byte bin_file;
						c:= !c + 1;
				end done; [||];
			with End_of_file -> close_in bin_file;
				let h_bmp = bmp_arr.(22) + bmp_arr.(23)*256 + bmp_arr.(24)*65536 + bmp_arr.(25)*16777216
				in let w_bmp = bmp_arr.(18) + bmp_arr.(19)*256 + bmp_arr.(20)*65536 + bmp_arr.(21)*16777216
				in let n = Array.make_matrix h_bmp w_bmp 0
				in for x = 0 to h_bmp - 1 do 
					for y = 0 to w_bmp - 1 do
						n.(h_bmp-x-1).(y) <- bmp_arr.(!d) + bmp_arr.(!d +1)*256 + bmp_arr.(!d +2)*65536;
						d := !d + 3;
					done ; done ; n

(* wyglad kamieni *)
		let black_stone = loadImage "bmp/black_stone.bmp"
		let white_stone = loadImage "bmp/white_stone.bmp"

(* zapis stanu gry *)
		let save_log = open_out "othello.gamelog"
		
		let gui_save board who =
			let f pos fld = ((match fld with
					1 -> output save_log "X" 0 1 |
					-1 -> output save_log "O" 0 1 |
					_ -> output save_log "_" 0 1); if pos.px = 8 then output save_log "\n" 0 1)
			in iterator f board
			
		let gui_gamelog str = 
			output save_log (str^"\n") 0 (String.length str + 1)

(* menu pomocy *)
		let help () = 
			Graphics.fill_rect (gs_x) (gs_y) (weight*9) (height*9);
			Graphics.set_color Graphics.white;
			Graphics.moveto (gs_x + 10) (gs_y + height*9 - 20);
			Graphics.draw_string "e - Exit";
			Graphics.moveto (gs_x + 10) (gs_y + height*9 - 35);
			Graphics.draw_string "n - New game";
			Graphics.moveto (gs_x + 10) (gs_y + height*9 - 50);
			Graphics.draw_string "b - Back move";
			Graphics.moveto (gs_x + 10) (gs_y + height*9 - 65);
			Graphics.draw_string "r - Resign";
			Graphics.moveto (gs_x + 10) (gs_y + height*9 - 80);
			Graphics.draw_string "h - This menu";
			Graphics.moveto (gs_x + 10) (gs_y + height*9 - 95);
			Graphics.draw_string "Press any key...";
			Graphics.set_color Graphics.black;
			Graphics.moveto 0 0;
			(fun x -> ()) (Graphics.wait_next_event [Graphics.Button_down; Graphics.Key_pressed])

(* 
 * gui_key i gui_mouse - dobre miejsce na kontrole funkcji move
 *)
		
(* gdy klawisz *)
		let gui_key board ch who =
			match ch with 
				'e' -> raise Event_Exit      |  (* chcemy wyjsc      *)
				'n' -> raise Event_Reset     |  (* chcemy od nowa    *)
				'b' -> raise Event_Back      |  (* chcemy cofnac     *)
				'r' -> raise Event_Surrender |  (* chcemy sie poddac *)
				'h' -> help (); raise Event_Empty | (* chcemy pomocy *)
				 _  -> raise Event_Empty        (* nic nie robimy    *)

(* gdy mysz *)
		let gui_mouse board mousepos who =
			let x = (fst mousepos - gs_x)/weight
			in let y = (snd mousepos - gs_y)/height
			in if ((x > 8) || (x < 1) || (y > 8) || (x < 1))
				then raise Event_Empty (* klik poza zakresem - nic nierobimy *)
				else (* zwrocimy nowa plansze o ile ruch jest poprawny *)
					let m = { mx = x; my = y }
					in if (isLegal board m who)
						then if (legal board who) = []
							then pass board who
							else move board m who
						else raise Event_Empty

(* aby pokazac : plansza -> (czy dla mnie, czy dla przeciwnika) -> unit *)
		let gui_show board who =
			Graphics.clear_graph ();
			(iterator
				(fun pos field -> 
					begin 
						(match field with
							-1 -> Graphics.draw_image (Graphics.make_image white_stone)
								(gs_x + (pos.px*weight)) (gs_y + (pos.py*height)) |
							1  -> Graphics.draw_image (Graphics.make_image black_stone)
								(gs_x + (pos.px*weight)) (gs_y + (pos.py*height)) |
							_  -> ());
						Graphics.draw_rect (gs_x + (pos.px*weight)) (gs_y + (pos.py*height)) weight height;
					end) board);
			Graphics.moveto (gs_x + weight + 10) (gs_y + height * 9 + 15);
			Graphics.draw_string
				(if (isEndGame board)
					then ((match (gameResult board) with
						Win x -> if x then "Player 1 (black) wins "^(string_of_int board.fp)^":"^(string_of_int board.sp) 
							else "Player 2 (white) wins "^(string_of_int board.sp)^":"^(string_of_int board.fp) |
						_ -> "Remis")^"   Press any key...")
					else (if who then "Player 1 (black) move" else "Player 2 (white) move"));
			Graphics.moveto 0 0;
			(List.iter
				(function el -> 
					(Graphics.set_color Graphics.red;
					Graphics.fill_circle 
						(gs_x + weight/2 + (el.mx*weight)) (gs_y + height/2 + (el.my*height)) 4;
					Graphics.set_color Graphics.black)) 
				(legal board who))

	end;;
