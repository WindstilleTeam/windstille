// FIXME: Global variables are evil, so never use: "foo <- 5", instead use "local foo = 5"

// The following two variables are evil, they should be either
// declared in a sector specific file or stored somewhere on the C++
// side. These variables are 'special' because they contain state that
// survise the execution of the character script

// Used in data/sector/newformat2/bob.nut
bob_seen_before <- false;

// Used in data/sector/newformat2/bob.nut
bob_knows_you_mercenary <- false;

