(define (huffman l)
  (define (proces cd)
    (if (= (huff-size cd) 1)
        (huff-first cd)
        (let ((t1 (huff-first cd))
              (cd1 (huff-remove cd)))
          (let ((t2 (huff-first cd1))
                (cd2 (huff-remove cd1)))
            (proces (huff-put cd2 (merge t1 t2)))))))
  (proces (make-huff-cd l)))

(define leaf cons)
(define frequemey car)
(define (merge t1 t2)
  (list (+ (frequemey t1) (frequemey t2)) t1 t2))
(define left cadr)
(define right caddr)
(define (leaf? t) (not (pair? (cdr t))))
(define (make-huff-cd l)
  (define (make-queque q l)
    (if (null? l) q
        (make-queque (put q (car l)) (cdr l))))
  (make-queque empty l)) ; empty)
(define (huff-size q)
  (+ (size (car q)) (size (cdr q))))
(define (huff-put q l)
  (cons (car q) (put (cdr q) t)))
(define (huff-first q)
  (cond ((empty? (car q)) (first (cdr q)))
        ((empty? (cdr q)) (first (car q)))
        (else (let ((f1 (first (car q)))
                    (f2 (first (cdr q))))
                (if (< (frequemey f1) (frequemey f2)) f1 f2)))))
(define (huff-remove q)
  (cond ((empty? (car q)) (cons empty (remove (cdr q))))
        ((empty? (cdr q)) (cons (remove car q) empty))
        (else (let ((f1 (first (car q)))
                    (f2 (first (cdr q))))
                (if (< (frequemey f1) (frequemey f2))
                    (cons (remove (car q)) (cdr q))
                    (cons (car q) (remove (cdr q))))))))