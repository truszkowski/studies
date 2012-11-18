(define (heap-sort l) (getall null (putall empty l)))
(define (putall q l)
  (if (null? l)
      q
      (putall (put q (car l)) (cdr l))))
(define (getall l q)
  (if (empty? q)
      l
      (let ((gm (getmax q)))
        (getall (cons (car gm) l) (cdr gm)))))
(define root car)
(define (size h)
  (if (null? h)
      0
      (cadr h)))
(define left caddr)
(define right cadddr)
(define (heap v l r)
  (list v (+ (size l) (size r) 1) l r))
(define (put h x)
  (cond ((empty? h)
         (heap x empty empty))
        ((< (size (left h)) (size (right h)))
         (heap (max x (root h)) (put (left h) (min x (root h))) (right h)))
        (else
         (heap (max x (root h)) (left h) (put (right h) (min x (root h)))))))
(define (getmax h)
  (cond ((empty? h)
         (error "pusta kolejka"))
        ((empty? (left h))
         (cons (root h) (right h)))
        ((empty? (right h))
         (cons (root h) (left h)))
        ((>= (root (left h)) (root (right h)))
         (let ((maxleft (getmax (left h))))
           (cons (root h) (heap (car maxleft) (cdr maxleft) (right h)))))
        (else 
         (let ((maxright (getmax (right h))))
           (cons (root h) (heap (car maxright) (left h) (cdr maxright)))))))