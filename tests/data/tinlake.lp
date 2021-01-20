Maximize
 +10000 tinInvest +1000 dropInvest +100000 tinRedeem +1000000 dropRedeem
Subject To
 c1: +tinInvest +dropInvest -tinRedeem -dropRedeem >= -200
 c2: +tinInvest +dropInvest -tinRedeem -dropRedeem <= 9800
 c3: +0.85 tinInvest -0.15 dropInvest -0.85 tinRedeem +0.15 dropRedeem >= -50
 c4: -0.8 tinInvest +0.2 dropInvest +0.8 tinRedeem -0.2 dropRedeem >= 0
Bounds
0 <= tinInvest  <= 200
0 <= dropInvest <= 400
0 <= tinRedeem  <= 100
0 <= dropRedeem <= 300
End

