#!/bin/sh

cat <<EOF | xmenu  
[]= (Flextile) Tiled 	0
><> (Flextile) Floating	1
[M] (Flextile) Monocle 	2
||| (Flextile) Columns 	3
>M> (Flextile) Floating Master	4
[D] (Flextile) Deck	5
TTT (Flextile) Bottom Stack	6
=== (Flextile) Bottom Stack Horizontal	7
|M| (Flextile) Centered Naster	8
-M- (Flextile) Centered Master Horizontal	9
::: (Flextile) Gapless Grid	10
[\] (Flextile) Fibonacci Dwindle	11
(@) (Flextile) Fibonacci Spiral	12
[T] (Flextile) Tatami Tats	13
[]= Tiled	14
[M] Monocle	15
TTT Bottom Stack	16
=== Bottom Stack Horizontal	17
|M| Centered Master	18
>M> Centered Floating Master	19
||| Columns	20
[D] Deck	21
(@) Fibonacci Spiral	22
[\\] Fibonacci Dwindle	23
HHH Grid	24
--- Horizontal Grid	25
::: Gapless Grid	26
### N-row Grid	27
EOF

