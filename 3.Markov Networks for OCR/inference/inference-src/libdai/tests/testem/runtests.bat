testem 2var.fg 2var_data.tab 2var.em > testem.out.tmp
testem 3var.fg 2var_data.tab 3var.em >> testem.out.tmp
testem ..\hoi1.fg hoi1_data.tab hoi1_share_f0_f2.em >> testem.out.tmp
testem ..\hoi1.fg hoi1_data.tab hoi1_share_f0_f1_f2.em >> testem.out.tmp
diff -s testem.out.tmp testem.out

del testem.out.tmp
