[2008-08-XX] Chj: Naming rule:
Names of functions, structs, constants in this gadgetlib should all start with ggt_ .

[2008-08-11] Chj:
The interfaces here should strive to be as independent as possible, e.g. use standard types
such as int, long, and a special one "__int64" etc. Don't define complex struct and try
not to draw in trivial headers for users.

[2011-10-25] Defining self-sufficient, although complex, structs should be allowed.
It does not hurt the user from adopting gadgetlib. Yes, I think so today. --Chj
