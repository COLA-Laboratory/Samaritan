#include "../headers/global.h"
#include "../headers/metaheuristics.h"
#include "../headers/crossover.h"
#include "../headers/mutation.h"
#include "../headers/problem.h"
#include "../headers/print.h"
#include "../headers/analysis.h"
#include "../headers/sort.h"



static int partition_by_obj(SMRT_individual * pop_table, int pop_sort[], int left, int right, int obj_index)
{
    double temp_obj = pop_table[pop_sort[left]].obj[obj_index];
    int temp_index = pop_sort[left];
    while(left < right)
    {
        while ((left < right) && (pop_table[pop_sort[right]].obj[obj_index] >= temp_obj))right--;
        if (left < right)
        {
            pop_sort[left] = pop_sort[right];
            left++;
        }
        while ((left < right) && (pop_table[pop_sort[left]].obj[obj_index] < temp_obj))left++;
        if (left < right)
        {
            pop_sort[right] = pop_sort[left];
            right--;
        }
    }
    pop_sort[left] = temp_index;
    return left;
}


extern void quicksort_by_obj(SMRT_individual* pop_table, int pop_sort[], int left, int right, int obj_index)
{
    int pos = 0;

    if (left < right)
    {
        pos = partition_by_obj(pop_table, pop_sort, left, right, obj_index);
        quicksort_by_obj(pop_table, pop_sort, pos + 1, right, obj_index);
        quicksort_by_obj(pop_table, pop_sort, left, pos - 1, obj_index);
    }
    return;
}

/*对某一个rank的solution按照某一objective进行排序，返回当前rank的solution的个数*/
static int sort_by_obj_rank(SMRT_individual *pop_table, int sort_arr[], int obj_index, int rank_index, int pop_num)
{
    int i = 0, j = 0;
    int array_num = 0;

    for (i = 0; i < pop_num; i++)
    {
        if (pop_table[i].rank == rank_index)
        {
            sort_arr[array_num++] = i;
        }
    }

    quicksort_by_obj(pop_table, sort_arr, 0, array_num - 1, obj_index);

    return array_num;
}



extern void setDistance_by_index(Distance_info_t *distance_arr, int index, int pop_num, double distance)
{
    int k = 0;
    for (k = 0; k < pop_num; k++)
    {
        if (distance_arr[k].idx == index)
        {
            distance_arr[k].E_distance += distance;
        }
    }
    return;
}




/*这个函数写的复杂了*/
extern int crowding_distance_assign(SMRT_individual *pop_table, int pop_sort[], int pop_num,  int rank_index)
{
    int i = 0, j = 0, k = 0;
    int pop_num_in_rank = 0;
    int *sort_arr = NULL;
    Distance_info_t *distance_arr;


    distance_arr  = (struct distance_info_t*) malloc(sizeof(Distance_info_t) * pop_num);
    if (NULL == distance_arr)
    {
        goto CROWDING_DISTANCE_FAIL_HANDLE;
    }
    sort_arr = (int*)malloc(sizeof(int) * pop_num);
    if (NULL == sort_arr)
    {
        goto CROWDING_DISTANCE_FAIL_HANDLE;
    }

    /*找出所有对应rank的值*/
    for (i = 0; i < pop_num; i++)
    {
        if (pop_table[i].rank == rank_index)
        {
            distance_arr[pop_num_in_rank++].idx = i;
        }
    }


    for (i = 0; i < g_algorithm_entity.algorithm_para.objective_number; i++)
    {
        memset(sort_arr, 0, sizeof(int) * pop_num);
        sort_by_obj_rank(pop_table, sort_arr, i, rank_index, pop_num);

        /*第一个和最后一个赋值为无穷大，为了使其能够保存下来*/
        pop_table[sort_arr[0]].fitness = INF;
        setDistance_by_index(distance_arr, sort_arr[0], pop_num_in_rank, INF);
        pop_table[sort_arr[pop_num_in_rank - 1]].fitness = INF;
        setDistance_by_index(distance_arr, sort_arr[pop_num_in_rank - 1], pop_num_in_rank, INF);
        for (j = 1; j < pop_num_in_rank - 1; j++)
        {
            if (INF != pop_table[sort_arr[j]].fitness)
            {
                if (pop_table[sort_arr[pop_num_in_rank - 1]].obj[i] == pop_table[sort_arr[0]].obj[i])
                {
                    pop_table[sort_arr[j]].fitness += 0;
                }
                else
                {
                    pop_table[sort_arr[j]].fitness += (pop_table[sort_arr[j+1]].obj[i] - pop_table[sort_arr[j - 1]].obj[i]) / (pop_table[sort_arr[pop_num_in_rank - 1]].obj[i] - pop_table[sort_arr[0]].obj[i]);
                    setDistance_by_index(distance_arr, sort_arr[j], pop_num_in_rank, pop_table[sort_arr[j]].fitness);
                }
            }
        }
    }

    distance_quick_sort(distance_arr, 0, pop_num_in_rank - 1);
    for (i = 0; i < pop_num_in_rank; i++)
    {
        pop_sort[i] = distance_arr[i].idx;
    }


    CROWDING_DISTANCE_FAIL_HANDLE:
    free(distance_arr);
    free(sort_arr);
    return pop_num_in_rank;
}





static void NSGA2_select(SMRT_individual *parent_pop, SMRT_individual *merge_pop)
{
    int i = 0, sort_num = 0;
    int *pop_sort = NULL;
    int merge_pop_number = 0, current_pop_num = 0, temp_number = 0, rank_index = 0;

    merge_pop_number = 2 * g_algorithm_entity.algorithm_para.pop_size;
    pop_sort = (int*)malloc(sizeof(int) * merge_pop_number);
    if (NULL == pop_sort)
    {
        printf("malloc failed in the pop_sort\n");
        goto NSGA2_SELECT_TERMINATE_HANDLE;
    }

    non_dominated_sort(merge_pop, merge_pop_number);

    while (1)
    {
        temp_number = 0;
        for (i = 0; i < merge_pop_number; i++)
        {
            if (merge_pop[i].rank == rank_index)
            {
                temp_number++;
            }
        }
        if (current_pop_num + temp_number <= g_algorithm_entity.algorithm_para.pop_size)
        {
            for (i = 0; i < merge_pop_number; i++)
            {
                if (merge_pop[i].rank == rank_index)
                {
                    copy_individual(merge_pop + i, parent_pop + current_pop_num);
                    current_pop_num++;
                }
            }
            rank_index++;
        }
        else
            break;
    }

    if (current_pop_num == g_algorithm_entity.algorithm_para.pop_size)
    {
        goto NSGA2_SELECT_TERMINATE_HANDLE;
    }
    else
    {
        sort_num = crowding_distance_assign(merge_pop, pop_sort, merge_pop_number, rank_index);
        /*这一行有点问题，出现了SIGSEG*/
        while(1)
        {
            /*对最后一层rank的solution，计算distance后在依据distance值纳入下一代*/
            if (current_pop_num < g_algorithm_entity.algorithm_para.pop_size)
            {
                copy_individual(merge_pop + pop_sort[--sort_num], parent_pop + current_pop_num);
                current_pop_num++;
            }
            else {
                break;
            }
        }
    }
    for(i = 0;i<g_algorithm_entity.algorithm_para.pop_size;i++)
    {
        parent_pop[i].fitness = 0;
    }

NSGA2_SELECT_TERMINATE_HANDLE:
    free(pop_sort);
    return ;
}


extern void NSGA2_framework (SMRT_individual *parent_pop, SMRT_individual *offspring_pop, SMRT_individual *mixed_pop)
{
    g_algorithm_entity.iteration_number                  = 1;
    g_algorithm_entity.algorithm_para.current_evaluation = 0;
    printf ("Progress: 1%%");

    // initialize population
    initialize_population_real (parent_pop, g_algorithm_entity.algorithm_para.pop_size);
    evaluate_population (parent_pop, g_algorithm_entity.algorithm_para.pop_size);


    // track the current evolutionary progress, including population and metrics
    track_evolution (parent_pop, g_algorithm_entity.iteration_number, 0);

    while (g_algorithm_entity.algorithm_para.current_evaluation < g_algorithm_entity.algorithm_para.max_evaluation)
    {

        g_algorithm_entity.iteration_number++;
        print_progress ();

        // reproduction (crossover and mutation)
        crossover_nsga2 (parent_pop, offspring_pop);
        mutation_pop(offspring_pop);
        evaluate_population (offspring_pop, g_algorithm_entity.algorithm_para.pop_size);

        // environmental selection
        merge_population(mixed_pop, parent_pop, g_algorithm_entity.algorithm_para.pop_size, offspring_pop, g_algorithm_entity.algorithm_para.pop_size);
        NSGA2_select(parent_pop, mixed_pop);

        // track the current evolutionary progress, including population and metrics
        track_evolution (parent_pop, g_algorithm_entity.iteration_number, g_algorithm_entity.algorithm_para.current_evaluation >= g_algorithm_entity.algorithm_para.max_evaluation);
    }

    return;
}