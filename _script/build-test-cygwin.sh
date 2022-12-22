#!/bin/bash

target_board=("renesas_t1v1")
Compilers=("CCRH")
HV_OBJS=("obj_1pe" "obj_2pe" "obj_2pe_mvm" "obj_2pe_svm" "obj_1pe_com" "obj_1pe_atk2" "obj_2pe_atk2")
res=[]
if [[ $# > 1 ]]; then
    if [[ "$1" == "-a" ]] && [[ "$1" == "--all" ]]; then
        target_board=("renesas_t1v1" "sunny_gpxu2a" "renesas_rcar_s4")
        Compilers=("CCRH" "GHS")
        HV_OBJS=("obj_1pe" "obj_2pe" "obj_2pe_mvm" "obj_2pe_svm" "obj_1pe_com" "obj_1pe_atk2" "obj_2pe_atk2")
    fi
fi

TorF () {
    if [[ $1 -eq 0 ]] ; then
        echo "True"
    else
        echo "False"
    fi
}

counter=0
for t in ${target_board[@]}; do
    for c in ${Compilers[@]}; do
        for h in ${HV_OBJS[@]}; do
            git clean -df || make _TARGET=$t COMPILER=$c HV_OBJ=$h clean
            make _TARGET=$t COMPILER=$c HV_OBJ=$h
            res[counter]=$(TorF $?); counter=$(($counter+1))
            git clean -df || make _TARGET=$t COMPILER=$c HV_OBJ=$h clean
        done
    done
done

counter=0
echo $RESULT
for t in ${target_board[@]}; do
    for c in ${Compilers[@]}; do
        for h in ${HV_OBJS[@]}; do
            echo "${res[counter]} : make _TARGET=$t COMPILER=$c HV_OBJ=$h"; counter=$(($counter+1))
        done
    done
    echo "" # Newline
done
