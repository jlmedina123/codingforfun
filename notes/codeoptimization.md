

http://leto.net/docs/C-optimization.php



### Inlining: 
`gcc -finline-functions ...` converts small functions to inline automatically

### Loop unrolling: 
`gcc -funroll-loops ...`: execute loop test less often

```
for (i = 0; i < 100; i++) do_stuff(i);

```

instead:
```
for (i = 0; i < 100; ) {
    do_stuff(i++); i++; 
    do_stuff(i++); i++;
    ... // 10 times
}
```

unrolled loop might not fit into instruction cache, which would make it slower

### Loop jamming
combine loops if index is same

### Loop inversion

`for (i = 1; i < MAX; i++)` replaced by `for (i = MAX; i !=0, i--)` or `i=MAX; while(i--)`

It might cause lookahead cache problems

### Strength reduction

Replace compute-intense expression with cheaper expressions

```
x = w % 8;
y = pow(x, 2.0);
z = y * 33;
for (i = 0; i < MAX; i++)  
{
      h = 14 * i; 
      printf("%d", h);
}
```

instead:

```
  x = w & 7;             /* bit-and cheaper than remainder */  
  y = x * x;             /* mult is cheaper than power-of */
  z = (y << 5) + y;      /* shift & add cheaper than mult */
  for (i = h = 0; i < MAX; i++) 
  {
      printf("%d", h);
      h += 14;           /* addition cheaper than mult */
  }
  
```  
  
### Avoid repeated use of expression
  
```
   total = 
    a->b->c[4]->aardvark +  
    a->b->c[4]->baboon +
    a->b->c[4]->cheetah +
    a->b->c[4]->dog;
```
  
instead:
  
```
    struct animals * temp = a->b->c[4];  
  total = 
    temp->aardvark +
    temp->baboon + 
    temp->cheetah +
    temp->dog;
 
```
 
 
### Most likely case first with if-then-else. But switch instead of large if-then-else

### Stack usage