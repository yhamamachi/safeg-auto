function setup_path {
    $ENV:Path+=";C:\ghs\comp_202214"
    $ENV:Path+=";C:\Program Files (x86)\Renesas Electronics\CS+\CC\CC-RH\V2.04.00\bin"
    $ENV:HLNK_DIR+=";C:\Program Files (x86)\Renesas Electronics\CS+\CC\CC-RH\V2.04.00"
    $ENV:Path="$HOME\scoop\apps\git\current\bin;"+$ENV:Path
}

function IsAdmin
 {
    $id = [System.Security.Principal.WindowsIdentity]::GetCurrent()
    $p = New-Object System.Security.Principal.WindowsPrincipal($id)
    return $p.IsInRole([System.Security.Principal.WindowsBuiltInRole]::Administrator)
 }
$IsAdmin = IsAdmin

# install scoop
gcm scoop -ea SilentlyContinue | Out-Null
if ($? -ne $true) { # command is missing
    Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
    if ( $IsAdmin -ne $true ) { # Not Admin
        irm get.scoop.sh | iex
    } else { # Admin
        iex "& {$(irm get.scoop.sh)} -RunAsAdmin"
    }
}

# install git using scoop
gcm git -ea SilentlyContinue | Out-Null
if ($? -ne $true) { # command is missing
    scoop install git
}

# install busybox using scoop
gcm busybox -ea SilentlyContinue | Out-Null
if ($? -ne $true) { # command is missing
    scoop install busybox
}

# install make/nm using scoop
gcm nm -ea SilentlyContinue | Out-Null
if ($? -ne $true) { # command is missing
    scoop install mingw
}

# install perl using scoop
gcm perl -ea SilentlyContinue | Out-Null
if ($? -ne $true) { # command is missing
    scoop install perl
}

# install python using scoop
gcm python -ea SilentlyContinue | Out-Null
if ($? -ne $true) { # command is missing
    scoop install python
}
# isntall pyyaml
pip install pyyaml

setup_path
echo "Setup is done"
