#include <sylvan.h>

int main() {
    int n_workers = 0; // auto-detect
    lace_init(n_workers, 40960000);
    lace_startup(0, NULL, NULL);

    sylvan_init_package(1LL << 20, 1LL << 25, 1LL << 20, 1LL << 25);
    sylvan_init_bdd();

    LACE_ME

    int CS = 0;
    int CS_PRIME = 1;
    int WAIT = 2;
    int WAIT_PRIME = 3;
    int FINISHED = 4;
    int FINISHED_PRIME = 5;

    int NUM_RELATIONS = 4;

    BDD init = sylvan_nithvar(CS);
    sylvan_protect(&init);
    init = sylvan_and(init, sylvan_ithvar(WAIT));
    init = sylvan_and(init, sylvan_nithvar(FINISHED));

    BDD map = sylvan_map_empty();
    sylvan_protect(&map);
    map = sylvan_map_add(map, CS_PRIME, sylvan_ithvar(CS));
    map = sylvan_map_add(map, WAIT_PRIME, sylvan_ithvar(WAIT));
    map = sylvan_map_add(map, FINISHED_PRIME, sylvan_ithvar(FINISHED));

    BDD vOld = sylvan_set_empty();
    sylvan_protect(&vOld);

    BDD vNew = init;
    sylvan_protect(&vNew);

    BDD rels[NUM_RELATIONS];
    BDD vars[NUM_RELATIONS];

    // enter relation

    BDD enter_pre = sylvan_and(sylvan_nithvar(CS), sylvan_ithvar(WAIT));
    sylvan_protect(&enter_pre);
    BDD enter_post = sylvan_and(sylvan_ithvar(CS_PRIME), sylvan_nithvar(WAIT_PRIME));
    sylvan_protect(&enter_post);

    BDD enter_vars = sylvan_set_empty();
    sylvan_protect(&enter_vars);
    enter_vars = sylvan_set_add(enter_vars, CS);
    enter_vars = sylvan_set_add(enter_vars, WAIT);

    rels[0] = sylvan_and(enter_pre, enter_post);
    vars[0] = enter_vars;

    // exit relation

    BDD exit_pre = sylvan_ithvar(CS);
    sylvan_protect(&exit_pre);
    BDD exit_post = sylvan_and(sylvan_nithvar(CS_PRIME), sylvan_ithvar(FINISHED_PRIME));
    sylvan_protect(&exit_post);

    BDD exit_vars = sylvan_set_empty();
    sylvan_protect(&exit_vars);
    exit_vars = sylvan_set_add(exit_vars, CS);
    exit_vars = sylvan_set_add(exit_vars, FINISHED);

    rels[1] = sylvan_and(exit_pre, exit_post);
    vars[1] = exit_vars;

    // leave relation

    BDD leave_pre = sylvan_nithvar(CS);
    sylvan_protect(&leave_pre);
    BDD leave_post = sylvan_set_empty();
    sylvan_protect(&leave_post);

    BDD leave_vars = sylvan_set_empty();
    sylvan_protect(&leave_vars);
    leave_vars = sylvan_set_add(leave_vars, CS);

    rels[2] = sylvan_and(leave_pre, leave_post);
    vars[2] = leave_vars;

    // restart relation

    BDD restart_pre = sylvan_ithvar(FINISHED);
    sylvan_protect(&restart_pre);
    BDD restart_post = sylvan_and(sylvan_ithvar(WAIT_PRIME), sylvan_nithvar(FINISHED_PRIME));
    sylvan_protect(&restart_post);

    BDD restart_vars = sylvan_set_empty();
    sylvan_protect(&restart_vars);
    restart_vars = sylvan_set_add(restart_vars, WAIT);
    restart_vars = sylvan_set_add(restart_vars, FINISHED);

    rels[3] = sylvan_and(restart_pre, restart_post);
    vars[3] = restart_vars;

    while (vOld != vNew) {
        printf("got here\n");

        vOld = vNew;

        int i;
        for (i = 0; i < NUM_RELATIONS; i++) {
            BDD relprod = sylvan_exists(sylvan_and(vNew, rels[i]), vars[i]);
            relprod = sylvan_compose(relprod, map);

            vNew = sylvan_or(vNew, relprod);

            printf("Did this %d times\n", i);
        }
    }

    //debug
    int count = mtbdd_satcount(vNew, 3);
    printf("SAT count: %d\n", count);

    FILE *f = fopen("test.dot", "w+");
    sylvan_fprintdot(f, vNew);
    fclose(f);

    sylvan_stats_report(stdout);
    sylvan_quit();
    lace_exit();
}