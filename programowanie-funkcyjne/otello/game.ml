(*** G A M E ***)

module Othello = BoardGame 
                   (OthelloBoard) 
                   (OthelloAI (OthelloBoard))
                   (OthelloGUI (OthelloBoard))
;;
