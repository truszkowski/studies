(define punkt cons)                           ; Definicja punktu
(define prosta list)                          ; Definicja prostej
(define punkt-x car)                          ; Wartosc punktu na osi x
(define punkt-y cdr)                          ; Wartosc punktu na osi y
(define a car)                                ; Wspolczynnik a prostej
(define b cadr)                               ; Wspolczynnik b prostej
(define strona caddr)                         ; Kierunej zginania
(define g procedure?)                         ; Definicja zgiecia w gore
(define d list)                               ; Definicja zgiecia w dol
(define (g? prosta) (eq? (strona prosta) g))  ; Procedura sprawdzajaca czy zagina w gore
(define (origami lista)                       ; Procedura origami  
  (define (origami* lista xp yp)
    (if (null? lista) 1
        (let* ((a (a (car lista))) (b (b (car lista))) (fxp (+ b (* a xp)))
               (sx (- (* 2 (/ (+ xp (* a yp) (* (- a) b)) (+ (* a a) 1))) xp))
               (sy (- (* 2 (/ (+ (* a xp) (* a a yp) b) (+ (* a a) 1))) yp)))
          (cond ((= yp fxp) (origami* (cdr lista) xp yp))
                ((and (g? (car lista)) (< yp fxp)) 0)
                (else (+ (origami* (cdr lista) xp yp) (origami* (cdr lista) sx sy)))))))
  (lambda (punkt) (origami* (reverse lista) (punkt-x punkt) (punkt-y punkt))))