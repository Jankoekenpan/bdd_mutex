#include <sylvan.h>
#include <stdio.h>
#include <float.h>

enum Var {
    WAIT,
    WAIT_PRIME,
    CS,
    CS_PRIME,
    FINISHED,
    FINISHED_PRIME
};

BDD makeMap() {
    LACE_ME;

    BDD map = sylvan_map_empty();
    map = sylvan_map_add(map, WAIT_PRIME, sylvan_ithvar(WAIT));
    map = sylvan_map_add(map, CS_PRIME, sylvan_ithvar(CS));
    map = sylvan_map_add(map, FINISHED_PRIME, sylvan_ithvar(FINISHED));

    return map;
}

BDD makeVariables() {
    LACE_ME;

    BDD set = sylvan_set_empty();
    set = sylvan_set_add(set, WAIT);
    set = sylvan_set_add(set, CS);
    set = sylvan_set_add(set, FINISHED);

    return set;
}

BDD and3(BDD one, BDD two, BDD three) {
    LACE_ME;

    BDD r = sylvan_and(one, two);
    r = sylvan_and(r, three);
    return r;
}

BDD and6(BDD one, BDD two, BDD three, BDD four, BDD five, BDD six) {
    LACE_ME;

    BDD x = and3(one, two, three);
    BDD y = and3(four, five, six);

    return sylvan_and(x, y);
}

BDD initialState() {
    LACE_ME;

    BDD wait        = sylvan_ithvar(WAIT);
    BDD cs          = sylvan_nithvar(CS);
    BDD finished    = sylvan_nithvar(FINISHED);

    return and3(wait, cs, finished);
}

BDD enteredState() {
    LACE_ME;

    BDD wait        = sylvan_nithvar(WAIT);
    BDD cs          = sylvan_ithvar(CS);
    BDD finished    = sylvan_nithvar(FINISHED);

    return and3(wait, cs, finished);
}

BDD exitedState() {
    LACE_ME;

    BDD wait        = sylvan_nithvar(WAIT);
    BDD cs          = sylvan_nithvar(CS);
    BDD finished    = sylvan_ithvar(FINISHED);

    return and3(wait, cs, finished);
}

BDD leftState() {
    LACE_ME;

    BDD wait        = sylvan_nithvar(WAIT);
    BDD cs          = sylvan_nithvar(CS);
    BDD finished    = sylvan_nithvar(FINISHED);

    return and3(wait, cs, finished);
}

BDD enter2(BDD set) {
    LACE_ME;

    BDD requirement_cs      = sylvan_nithvar(CS);
    BDD requirement_wait    = sylvan_ithvar(WAIT);

    BDD requirement_cs_prime    = sylvan_ithvar(CS_PRIME);
    BDD requirement_wait_prime  = sylvan_nithvar(WAIT_PRIME);

    BDD precondition    = sylvan_and(requirement_cs, requirement_wait);
    BDD postcondition   = sylvan_and(requirement_cs_prime, requirement_wait_prime);

    BDD relation = sylvan_and(precondition, postcondition);

    BDD vars = sylvan_set_empty();
    vars = sylvan_set_add(vars, CS);
    vars = sylvan_set_add(vars, WAIT);

    BDD exists = sylvan_exists(sylvan_and(set, relation), vars);
    BDD enter = sylvan_compose(exists, makeMap());
    return enter;
}

// BDD enterRelation(BDD set, BDD vars) {
//     LACE_ME;

//     BDD requirement_cs      = sylvan_nithvar(CS);
//     BDD requirement_wait    = sylvan_ithvar(WAIT);

//     BDD requirement_cs_prime    = sylvan_ithvar(CS_PRIME);
//     BDD requirement_wait_prime  = sylvan_nithvar(WAIT_PRIME);

//     BDD precondition    = sylvan_and(requirement_cs, requirement_wait);
//     BDD postcondition   = sylvan_and(requirement_cs_prime, requirement_wait_prime);

//     BDD relation = sylvan_and(precondition, postcondition);
//     return sylvan_exists(sylvan_and(set, relation), vars);
// }

BDD exit2(BDD set) {
    LACE_ME;

    BDD requirement_cs      = sylvan_ithvar(CS);

    BDD requirement_cs_prime        = sylvan_nithvar(CS_PRIME);
    BDD requirement_finished_prime  = sylvan_ithvar(FINISHED_PRIME);

    BDD precondition  = requirement_cs;
    BDD postcondition = sylvan_and(requirement_cs_prime, requirement_finished_prime);

    BDD relation = sylvan_and(precondition, postcondition);

    BDD vars = sylvan_set_empty();
    vars = sylvan_set_add(vars, CS);
    vars = sylvan_set_add(vars, FINISHED);

    BDD exists = sylvan_exists(sylvan_and(set, relation), vars);
    BDD exit = sylvan_compose(exists, makeMap());
    return exit;
}

// BDD exitRelation(BDD set, BDD vars) {
//     LACE_ME;

//     BDD requirement_cs      = sylvan_ithvar(CS);

//     BDD requirement_cs_prime        = sylvan_nithvar(CS_PRIME);
//     BDD requirement_finished_prime  = sylvan_ithvar(FINISHED_PRIME);

//     BDD precondition  = requirement_cs;
//     BDD postcondition = sylvan_and(requirement_cs_prime, requirement_finished_prime);

//     BDD relation = sylvan_and(precondition, postcondition);
//     return sylvan_exists(sylvan_and(set, relation), vars);
// }

BDD leave2(BDD set) {
    LACE_ME;

    BDD postcondition = sylvan_nithvar(CS_PRIME);

    BDD vars = sylvan_set_empty();
    vars = sylvan_set_add(vars, CS);

    BDD exists = sylvan_exists(sylvan_and(set, postcondition), vars);
    BDD leave = sylvan_compose(exists, makeMap());
    return leave;
}

// BDD leaveRelation(BDD set, BDD vars) {
//     LACE_ME;

//     BDD postcondition = sylvan_nithvar(CS_PRIME);

//     return sylvan_exists(sylvan_and(set, postcondition), vars);
// }

BDD restart2(BDD set) {
    LACE_ME;

    BDD requirement_finished        = sylvan_ithvar(FINISHED);

    BDD requirement_wait_prime      = sylvan_ithvar(WAIT_PRIME);
    BDD requirement_finished_prime  = sylvan_nithvar(FINISHED_PRIME);

    BDD precondition = requirement_finished;
    BDD postcondition = sylvan_and(requirement_wait_prime, requirement_finished_prime);

    BDD relation = sylvan_and(precondition, postcondition);

    BDD vars = sylvan_set_empty();
    vars = sylvan_set_add(vars, FINISHED);
    vars = sylvan_set_add(vars, FINISHED_PRIME);

    BDD exists = sylvan_exists(sylvan_and(set, relation), vars);
    BDD restart = sylvan_compose(exists, makeMap());
    return restart;
}

// BDD restartRelation(BDD set, BDD vars) {
//     LACE_ME;

//     BDD requirement_finished        = sylvan_ithvar(FINISHED);

//     BDD requirement_wait_prime      = sylvan_ithvar(WAIT_PRIME);
//     BDD requirement_finished_prime  = sylvan_nithvar(FINISHED_PRIME);

//     BDD precondition = requirement_finished;
//     BDD postcondition = sylvan_and(requirement_wait_prime, requirement_finished_prime);

//     BDD relation = sylvan_and(precondition, postcondition);
//     return sylvan_exists(sylvan_and(set, relation), vars);
// }

const int RELATION_COUNT = 4;
BDD (*p[4]) (BDD set);

int main() {
    //initialize function pointer array (the relations)
    p[0]    = enter2;
    p[1]    = exit2;
    p[2]    = leave2;
    p[3]    = restart2;


    int n_workers = 0; // auto-detect
    lace_init(n_workers, 40960000);
    lace_startup(0, NULL, NULL);

    // use at most 512 MB, nodes:cache ratio 2:1, initial size 1/32 of maximum
    sylvan_set_limits(512*1024*1024, 1, 5);
    sylvan_init_package();
    sylvan_init_mtbdd();

    /* TODO ... do stuff ... */
    LACE_ME;

    BDD variables       = makeVariables();  sylvan_protect(&variables);
    
    BDD initState       = initialState();   sylvan_protect(&initState);
    BDD enterState      = enteredState();   sylvan_protect(&enterState);
    BDD exitState       = exitedState();    sylvan_protect(&exitState);
    BDD leaveState      = leftState();      sylvan_protect(&leaveState);

    double count = sylvan_satcount(initState, variables);
    printf("initState satcount = %f\r\n", count);

    //TODO implement algorithm to generate state-space o.0
    BDD Vold = sylvan_set_empty();
    BDD Vnew = sylvan_set_add(sylvan_set_empty(), initState);

    sylvan_protect(&Vold);
    sylvan_protect(&Vnew);

    while (&Vold != &Vnew) {
        Vold = Vnew;

        int i = 0;
        for (i; i < RELATION_COUNT; i++) {
            BDD newReachableStates = (*p[i]) (Vnew);
            Vnew = sylvan_or(Vnew, newReachableStates);
        }
    }

    count = sylvan_satcount(Vnew, variables);
    printf("result satcount = %f\r\n", count);

    //TODO look at sylvan_protect and sylvan_unprotect
    //TODO uses pointers to BDDs!
    //TODO use sylvan_compose? where? how? it's used for function composition?

    sylvan_unprotect(&Vold);
    sylvan_unprotect(&Vnew);

    sylvan_unprotect(&initState);
    sylvan_unprotect(&enterState);
    sylvan_unprotect(&exitState);
    sylvan_unprotect(&leaveState);

    sylvan_stats_report(stderr);
    sylvan_quit();
    lace_exit();

    return 0;
}