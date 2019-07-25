#include "../headers/global.h"
#include "../headers/crossover.h"
#include "../headers/mating.h"
#include "../headers/random.h"
#include "../headers/memory.h"
#include "../headers/dominance_relation.h"
#include "../headers/population.h"


extern void crossover_IBEA(SMRT_individual *parent_pop_table, SMRT_individual *offspring_pop_table)
{
    int i, temp, rand;
    int *a1, *a2;
    SMRT_individual *parent1, *parent2;

    int *flage_arr = NULL;
    double *figcomp = NULL;


    figcomp = (double*)malloc(sizeof(double) * g_algorithm_entity.algorithm_para.pop_size* g_algorithm_entity.algorithm_para.pop_size * 4);
    if (NULL == figcomp)
    {
        printf("malloc indicator fitness failed\n");
        goto IBEA_CROSSOVER_TERMINATE_HANDLE;
    }
    a1 = (int *) malloc (g_algorithm_entity.algorithm_para.pop_size * sizeof(int));
    a2 = (int *) malloc (g_algorithm_entity.algorithm_para.pop_size * sizeof(int));

    cal_indicator(parent_pop_table, figcomp, g_algorithm_entity.algorithm_para.pop_size );


    for (i = 0; i < g_algorithm_entity.algorithm_para.pop_size; i++)
        a1[i] = a2[i] = i;

    for (i = 0; i < g_algorithm_entity.algorithm_para.pop_size; i++)
    {
        rand     = rnd (i, g_algorithm_entity.algorithm_para.pop_size - 1);
        temp     = a1[rand];
        a1[rand] = a1[i];
        a1[i]    = temp;
        temp     = a2[rand];
        a2[rand] = a2[i];
        a2[i]    = temp;
    }



    for (i = 0; i < g_algorithm_entity.algorithm_para.pop_size ; i += 4)
    {
        parent1 = tournament_by_fitness(&parent_pop_table[a1[i]], &parent_pop_table[a1[i + 1]], LESSER);
        parent2 = tournament_by_fitness(&parent_pop_table[a1[i + 2]], &parent_pop_table[a1[i + 3]], LESSER);
        sbx_crossover (parent1, parent2, offspring_pop_table + i, offspring_pop_table + i + 1);
        parent1 = tournament_by_fitness(&parent_pop_table[a2[i]], &parent_pop_table[a2[i + 1]], LESSER);
        parent2 = tournament_by_fitness(&parent_pop_table[a2[i + 2]], &parent_pop_table[a2[i + 3]], LESSER);
        sbx_crossover (parent1, parent2, &offspring_pop_table[i + 2], &offspring_pop_table[i + 3]);
    }
IBEA_CROSSOVER_TERMINATE_HANDLE:
    free(a1);
    free(a2);
    free(figcomp);
    return;
    return;
}

extern void crossover_nsga2(SMRT_individual *parent_pop_table, SMRT_individual *offspring_pop_table)
{
    int i, temp, rand;
    int *a1, *a2;
    SMRT_individual *parent1, *parent2;

    a1 = (int *) malloc (g_algorithm_entity.algorithm_para.pop_size * sizeof(int));
    a2 = (int *) malloc (g_algorithm_entity.algorithm_para.pop_size * sizeof(int));
    for (i = 0; i < g_algorithm_entity.algorithm_para.pop_size; i++)
        a1[i] = a2[i] = i;

    for (i = 0; i < g_algorithm_entity.algorithm_para.pop_size; i++)
    {
        rand     = rnd (i, g_algorithm_entity.algorithm_para.pop_size - 1);
        temp     = a1[rand];
        a1[rand] = a1[i];
        a1[i]    = temp;
        temp     = a2[rand];
        a2[rand] = a2[i];
        a2[i]    = temp;
    }



    for (i = 0; i < g_algorithm_entity.algorithm_para.pop_size ; i += 4)
    {
        parent1 = tournament_NSGA2(&parent_pop_table[a1[i]], &parent_pop_table[a1[i + 1]]);
        parent2 = tournament_NSGA2(&parent_pop_table[a1[i + 2]], &parent_pop_table[a1[i + 3]]);
        sbx_crossover (parent1, parent2, offspring_pop_table + i, offspring_pop_table + i + 1);
        parent1 = tournament_NSGA2 (&parent_pop_table[a2[i]], &parent_pop_table[a2[i + 1]]);
        parent2 = tournament_NSGA2 (&parent_pop_table[a2[i + 2]], &parent_pop_table[a2[i + 3]]);
        sbx_crossover (parent1, parent2, &offspring_pop_table[i + 2], &offspring_pop_table[i + 3]);
    }

    free(a1);
    free(a2);
    return;
}


extern void crossover_spea2(SMRT_individual *elite_pop_table, SMRT_individual *offspring_pop_table)
{
    int i, temp, rand;
    int *a1, *a2;
    SMRT_individual *parent1, *parent2;

    a1 = (int *) malloc (g_algorithm_entity.algorithm_para.elite_pop_size * sizeof(int));
    a2 = (int *) malloc (g_algorithm_entity.algorithm_para.elite_pop_size * sizeof(int));
    for (i = 0; i < g_algorithm_entity.algorithm_para.elite_pop_size; i++)
        a1[i] = a2[i] = i;

    for (i = 0; i < g_algorithm_entity.algorithm_para.elite_pop_size; i++)
    {
        rand     = rnd (i, g_algorithm_entity.algorithm_para.elite_pop_size - 1);
        temp     = a1[rand];
        a1[rand] = a1[i];
        a1[i]    = temp;
        temp     = a2[rand];
        a2[rand] = a2[i];
        a2[i]    = temp;
    }



    for (i = 0; i < g_algorithm_entity.algorithm_para.elite_pop_size ; i += 4)
    {
        parent1 = tournament_by_fitness(&elite_pop_table[a1[i]], &elite_pop_table[a1[i + 1]], LESSER);
        parent2 = tournament_by_fitness(&elite_pop_table[a1[i + 2]], &elite_pop_table[a1[i + 3]], LESSER);
        sbx_crossover (parent1, parent2, offspring_pop_table + i, offspring_pop_table + i + 1);
        parent1 = tournament_by_fitness (&elite_pop_table[a2[i]], &elite_pop_table[a2[i + 1]], LESSER);
        parent2 = tournament_by_fitness (&elite_pop_table[a2[i + 2]], &elite_pop_table[a2[i + 3]], LESSER);
        sbx_crossover (parent1, parent2, &offspring_pop_table[i + 2], &offspring_pop_table[i + 3]);
    }

    free(a1);
    free(a2);
    return;
    return;
}


extern void crossover_MOEAD(SMRT_individual *parent_pop_table, SMRT_individual *offspring_pop_table)
{
    int i = 0, j = 0;
    int rand = 0, type = 0;
    int select_id[2] = {0};


    for(i = 0; i < g_algorithm_entity.algorithm_para.pop_size; i++)
    {
        rand = randomperc();
        if (rand < g_algorithm_entity.MOEAD_para.neighborhood_selection_probability)
        {
            type = NEIGHBOR;
        }
        else
        {
            type = GLOBAL_PARENT;
        }

        for (j = 0; j < 2; j++)
        {
            if (NEIGHBOR == type)
            {
                rand = rnd (0, g_algorithm_entity.MOEAD_para.neighbor_size - 1);
                select_id[j] = g_algorithm_entity.MOEAD_para.neighbor_table[i].neighbor[rand];
            }
            else
            {
                rand = rnd(0, g_algorithm_entity.algorithm_para.pop_size - 1);
                select_id[j] = rand;
            }

        }
       de_crossover(parent_pop_table + i, parent_pop_table + select_id[0],
                     parent_pop_table + select_id[1], offspring_pop_table + i);

    }

    return;
}

extern void crossover_MOEAD_dra(SMRT_individual *parent_pop_table, SMRT_individual *parent, int parent_index, SMRT_individual *offspring, NeighborType type)
{
    int i = 0;
    int rand = 0;
    int select_id[2] = {0};


    for (i = 0; i < 2; i++)
    {
        if (NEIGHBOR == type)
        {
            rand = rnd (0, g_algorithm_entity.MOEAD_para.neighbor_size - 1);
            select_id[i] = g_algorithm_entity.MOEAD_para.neighbor_table[parent_index].neighbor[rand];
        }
        else
        {
            rand = rnd(0, g_algorithm_entity.algorithm_para.pop_size - 1);
            select_id[i] = rand;
        }

    }
    de_crossover(parent, parent_pop_table + select_id[0],
                 parent_pop_table + select_id[1], offspring);



    return;
}

extern void crossover_SMSEMOA(SMRT_individual *parent_pop_table, SMRT_individual *offspring)
{
    int i, temp, rand;
    int *a1, *a2;
    SMRT_individual *parent1, *parent2, *offspring1, *offspring2;
    DOMINATE_RELATION dominateRelation;

    allocate_memory_for_ind (&offspring1);
    allocate_memory_for_ind (&offspring2);

    a1 = (int *) malloc (g_algorithm_entity.algorithm_para.pop_size * sizeof(int));
    a2 = (int *) malloc (g_algorithm_entity.algorithm_para.pop_size * sizeof(int));
    for (i = 0; i < g_algorithm_entity.algorithm_para.pop_size; i++)
        a1[i] = a2[i] = i;

    for (i = 0; i < g_algorithm_entity.algorithm_para.pop_size; i++)
    {
        rand     = rnd (i, g_algorithm_entity.algorithm_para.pop_size - 1);
        temp     = a1[rand];
        a1[rand] = a1[i];
        a1[i]    = temp;
        temp     = a2[rand];
        a2[rand] = a2[i];
        a2[i]    = temp;
    }

    parent1 = tournament_NSGA2(&parent_pop_table[a1[0]], &parent_pop_table[a1[1]]);
    parent2 = tournament_NSGA2(&parent_pop_table[a1[2]], &parent_pop_table[a1[3]]);
    sbx_crossover (parent1, parent2, offspring1, offspring2);

    dominateRelation = check_dominance(offspring1, offspring2);


    if (DOMINATED == dominateRelation)
    {
        copy_individual(offspring1, offspring);
    }
    else if (DOMINATE == dominateRelation)
    {
        copy_individual(offspring2, offspring);
    }
    else
    {
        copy_individual(offspring1, offspring);
    }

    destroy_memory_for_ind(offspring1);
    destroy_memory_for_ind(offspring2);

    return;
}


extern void crossover_HypE(SMRT_individual *parent_pop_table, SMRT_individual *offspring1, SMRT_individual *offspring2)
{
    int i, temp, rand;
    int *a1, *a2;
    SMRT_individual *parent1, *parent2;
    DOMINATE_RELATION dominateRelation;


    a1 = (int *) malloc (g_algorithm_entity.algorithm_para.pop_size * sizeof(int));
    a2 = (int *) malloc (g_algorithm_entity.algorithm_para.pop_size * sizeof(int));
    for (i = 0; i < g_algorithm_entity.algorithm_para.pop_size; i++)
        a1[i] = a2[i] = i;

    for (i = 0; i < g_algorithm_entity.algorithm_para.pop_size; i++)
    {
        rand     = rnd (i, g_algorithm_entity.algorithm_para.pop_size - 1);
        temp     = a1[rand];
        a1[rand] = a1[i];
        a1[i]    = temp;
        temp     = a2[rand];
        a2[rand] = a2[i];
        a2[i]    = temp;
    }

    parent1 = tournament_NSGA2(&parent_pop_table[a1[0]], &parent_pop_table[a1[1]]);
    parent2 = tournament_NSGA2(&parent_pop_table[a1[2]], &parent_pop_table[a1[3]]);
    sbx_crossover (parent1, parent2, offspring1, offspring2);


    return;
}