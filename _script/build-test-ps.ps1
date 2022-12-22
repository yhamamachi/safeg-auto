Param($Arg1)

$target_board = @("renesas_t1v1")
$Compilers = @("CCRH")
$HV_OBJS = @("obj_1pe", "obj_2pe", "obj_2pe_mvm", "obj_2pe_svm", "obj_1pe_com", "obj_1pe_atk2", "obj_2pe_atk2")
$res = New-Object 'object[,,]' $target_board.length,$Compilers.length,$HV_OBJS.length
if ( $Arg1 -like "-a") {
    $target_board = @("renesas_t1v1", "sunny_gpxu2a", "renesas_rcar_s4")
    $Compilers = @("CCRH")
    $HV_OBJS = @("obj_1pe", "obj_2pe", "obj_2pe_mvm", "obj_2pe_svm", "obj_1pe_com", "obj_1pe_atk2", "obj_2pe_atk2")
    $res = New-Object 'object[,,]' $target_board.length,$Compilers.length,$HV_OBJS.length
}


for ($t=0; $t -lt $target_board.length; $t++){
    for ($c=0; $c -lt $Compilers.length; $c++){
        for ($h=0; $h -lt $HV_OBJS.length; $h++){
            $_t = $target_board[$t]
            $_c = $Compilers[$c]
            $_h = $HV_OBJS[$h]
            git clean -df; if ($? -ne $true) { make _TARGET=$_t COMPILER=$_c HV_OBJ=$_h clean}
            make _TARGET=$_t COMPILER=$_c HV_OBJ=$_h
            $res[$t,$c,$h] = $?
            git clean -df; if ($? -ne $true) {make _TARGET=$_t COMPILER=$_c HV_OBJ=$_h clean}
        }
    }
}

echo $RESULT
for ($t=0; $t -lt $target_board.length; $t++){
    for ($c=0; $c -lt $Compilers.length; $c++){
        for ($h=0; $h -lt $HV_OBJS.length; $h++){
            $_t = $target_board[$t]
            $_c = $Compilers[$c]
            $_h = $HV_OBJS[$h]
            $_r = $res[$t,$c,$h]
            echo "$_r : make _TARGET=$_t COMPILER=$_c HV_OBJ=$_h"
        }
    }
        echo "" # Newline
}
