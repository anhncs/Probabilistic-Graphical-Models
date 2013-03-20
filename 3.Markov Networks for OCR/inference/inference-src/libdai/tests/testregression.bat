testall testfast.fg | sed "s/\(e[+-]\)0/\1/g" > testfast.out.tmp
diff -s testfast.out.tmp testfast.out
del testfast.out.tmp
