*<meta creator='lp_solve v5.5'>
*<meta rows=4>
*<meta columns=4>
*<meta equalities=0>
*<meta integers=4>
*<meta origsense='MAX'>
*
NAME          LPSolver
ROWS
 N  R0      
 G  c1      
 L  c2      
 G  c3      
 G  c4      
COLUMNS
    MARK0000  'MARKER'                 'INTORG'
    tinInves  R0        -10000.00000   c1        1.0000000000
    tinInves  c2        1.0000000000   c3        0.8500000000
    tinInves  c4        -0.800000000
    dropInve  R0        -1000.000000   c1        1.0000000000
    dropInve  c2        1.0000000000   c3        -0.150000000
    dropInve  c4        0.2000000000
    tinRedee  R0        -100000.0000   c1        -1.000000000
    tinRedee  c2        -1.000000000   c3        -0.850000000
    tinRedee  c4        0.8000000000
    dropRede  R0        -1000000.000   c1        -1.000000000
    dropRede  c2        -1.000000000   c3        0.1500000000
    dropRede  c4        -0.200000000
    MARK0001  'MARKER'                 'INTEND'
RHS
    RHS       c1        -200.0000000   c2        9800.0000000
    RHS       c3        -50.00000000
BOUNDS
 LO BND       tinInves  0.0000000000
 UP BND       tinInves  200.00000000
 LO BND       dropInve  0.0000000000
 UP BND       dropInve  400.00000000
 LO BND       tinRedee  0.0000000000
 UP BND       tinRedee  100.00000000
 LO BND       dropRede  0.0000000000
 UP BND       dropRede  300.00000000
ENDATA
