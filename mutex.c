#include <sylvan.h>
#include <stdio.h>
#include <float.h>

enum Var {
    WAIT,
    WAIT_NEXT,
    CS,
    CS_NEXT,
    FINISHED,
    FINISHED_NEXT
};

BDD makeMap() {
    LACE_ME;

    BDD map = sylvan_map_empty();
    map = sylvan_map_add(map, WAIT_NEXT, sylvan_ithvar(WAIT));
    map = sylvan_map_add(map, CS_NEXT, sylvan_ithvar(CS));
    map = sylvan_map_add(map, FINISHED_NEXT, sylvan_ithvar(FINISHED));

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

BDD enterRelation(BDD set, BDD vars) {
    LACE_ME;

    BDD requirement_cs      = sylvan_nithvar(CS);
    BDD requirement_wait    = sylvan_ithvar(WAIT);

    BDD requirement_cs_prime    = sylvan_ithvar(CS);
    BDD requirement_wait_prime  = sylvan_nithvar(WAIT);

    BDD precondition    = sylvan_and(requirement_cs, requirement_wait);
    BDD postcondition   = sylvan_and(requirement_cs_prime, requirement_wait_prime);

    BDD relation = sylvan_and(precondition, postcondition);
    return sylvan_exists(sylvan_and(set, relation), vars);
}

BDD exitRelation(BDD set, BDD vars) {
    LACE_ME;

    BDD requirement_cs      = sylvan_ithvar(CS);

    BDD requirement_cs_prime        = sylvan_nithvar(CS);
    BDD requirement_finished_prime  = sylvan_ithvar(FINISHED);

    BDD precondition  = requirement_cs;
    BDD postcondition = sylvan_and(requirement_cs_prime, requirement_finished_prime);

    BDD relation = sylvan_and(precondition, postcondition);
    return sylvan_exists(sylvan_and(set, relation), vars);
}

BDD leaveRelation(BDD set, BDD vars) {
    LACE_ME;

    BDD postcondition = sylvan_nithvar(CS);

    return sylvan_exists(sylvan_and(set, postcondition), vars);
}

BDD restartRelation(BDD set, BDD vars) {
    LACE_ME;

    BDD requirement_finished        = sylvan_ithvar(FINISHED);

    BDD requirement_wait_prime      = sylvan_ithvar(WAIT);
    BDD requirement_finished_prime  = sylvan_nithvar(FINISHED);

    BDD precondition = requirement_finished;
    BDD postcondition = sylvan_and(requirement_wait_prime, requirement_finished_prime);

    BDD relation = sylvan_and(precondition, postcondition);
    return sylvan_exists(sylvan_and(set, relation), vars);
}

const int RELATION_COUNT = 4;
BDD (*p[4]) (BDD set, BDD vars);

int main() {
    //initialize function pointer array (the relations)
    p[0]    = enterRelation;
    p[1]    = exitRelation;
    p[2]    = leaveRelation;
    p[3]    = restartRelation;


    int n_workers = 0; // auto-detect
    lace_init(n_workers, 40960000);
    lace_startup(0, NULL, NULL);

    // use at most 512 MB, nodes:cache ratio 2:1, initial size 1/32 of maximum
    sylvan_set_limits(512*1024*1024, 1, 5);
    sylvan_init_package();
    sylvan_init_mtbdd();

    /* TODO ... do stuff ... */
    LACE_ME;

    BDD variables       = makeVariables();
    
    BDD initState       = initialState();
    BDD enterState      = enteredState();
    BDD exitState       = exitedState();
    BDD leaveState      = leftState();

    double count = sylvan_satcount(initState, variables);
    printf("initState satcount = %f\r\n", count);

    //TODO implement algorithm to generate state-space o.0
    BDD Vold = sylvan_set_empty();
    BDD Vnew = sylvan_set_add(sylvan_set_empty(), initState);

    while (!sylvan_equiv(Vold, Vnew)) {
        Vold = Vnew;

        int i = 0;
        for (i; i < RELATION_COUNT; i++) {
            BDD newReachableStates = (*p[i]) (Vnew, variables);
            Vnew = sylvan_or(Vnew, newReachableStates);
        }
    }

    count = sylvan_satcount(Vnew, variables);
    printf("result satcount = %f\r\n", count);

    //TODO look at sylvan_protect and sylvan_unprotect
    //TODO uses pointers to BDDs!
    //TODO use sylvan_compose? where? how? it's used for function composition?

    sylvan_stats_report(stderr);
    sylvan_quit();
    lace_exit();

    return 0;
}