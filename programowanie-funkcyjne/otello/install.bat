
copy def.ml+board.mli+ai.mli+gui.mli+board_game.mli+board.ml+ai.ml+gui.ml+board_game.ml+game.ml debug.ml /B
copy debug.ml+othello.ml  tmp1.ml /B
copy debug.ml+lessons.ml  tmp2.ml /B
copy debug.ml+training.ml tmp3.ml /B

"C:\Program Files\Objective Caml\bin\ocamlc.opt.exe" -o othello.exe  graphics.cma tmp1.ml 
"C:\Program Files\Objective Caml\bin\ocamlc.opt.exe" -o lessons.exe  graphics.cma tmp2.ml
"C:\Program Files\Objective Caml\bin\ocamlc.opt.exe" -o training.exe graphics.cma tmp3.ml

del tmp1.* tmp2.* tmp3.* debug.ml
