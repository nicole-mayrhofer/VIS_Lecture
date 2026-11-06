import org.apache.tools.ant.taskdefs.condition.Os

plugins {
   id("cpp-application")
}

// ************************************************************
// ENTER Name of the sub project ...
// ************************************************************
val subProjectName: String = "Demo_SocketServer"
// ************************************************************
//
// ************************************************************

application {
    when {
        Os.isFamily(Os.FAMILY_WINDOWS) -> {
            targetMachines.set(listOf(machines.windows)) // baut für Windows Host-Architektur
        }

        Os.isFamily(Os.FAMILY_MAC) -> {
            targetMachines.set(listOf(machines.macOS)) // baut für macOS Host-Architektur
        }

        Os.isFamily(Os.FAMILY_UNIX) -> {
            targetMachines.set(listOf(machines.linux)) // baut für linux Host-Architektur
        }
        else -> {
            throw GradleException(":cpp:${subProjectName} run-target -> unknown OS family encountered")
        }
    }
}

tasks.register<Exec>("run") {
    // sicherstellen, dass erst gebaut wird
    dependsOn("build")
    group = "application"
    standardInput = System.`in`    // enable commandline input

    var exeFile : String?
    when {
        Os.isFamily(Os.FAMILY_WINDOWS) -> {
            exeFile = subProjectName + ".exe"
        }

        Os.isFamily(Os.FAMILY_MAC) -> {
            exeFile = subProjectName
        }

        Os.isFamily(Os.FAMILY_UNIX) -> {
            exeFile = subProjectName
        }
        else -> {
            throw GradleException(":cpp:${subProjectName} run-target -> unknown OS family encountered")
        }
    }
        // Pfad zum Executable bestimmen (im build/install/... Ordner)
    val exe = layout.buildDirectory.file(
        "exe/main/debug/$exeFile"
    )

    // Übergabeparameter beim Programmstart definieren ...
    args = listOf("1234")

    // Kommando setzen
    executable = exe.get().asFile.absolutePath
}


/*
// task to run cpp program
tasks.register("run-old", Exec::class) {
    dependsOn("build")         // make sure project has been built
    group = "application"                // set task group
    standardInput = System.`in`    // enable commandline input
    val bDir: String = layout.buildDirectory.get().toString()
    val exeDir:  String = "${bDir}/exe/main/debug/"
    var exeFile: String?
    when {
        Os.isFamily(Os.FAMILY_WINDOWS) -> {
            exeFile = subProjectName+ ".exe"
			commandLine("cmd", "/k", exeDir+exeFile) // start command in cmd shell
        }
        Os.isFamily(Os.FAMILY_MAC) -> {
            exeFile = subProjectName
			// some Macs can work without "bash" and "-c"
			commandLine("bash", "-c", exeDir+exeFile) // start command in cmd shell
        }
        Os.isFamily(Os.FAMILY_UNIX) -> {
            exeFile = subProjectName
			commandLine("bash", "-c", exeDir+exeFile) // start command in cmd shell
        }
        else -> { throw GradleException(":cpp:Hello run-target -> unknown OS family encountered")}
    }
}
*/

// task to run cpp program
tasks.register("kill", Exec::class) {
    group = "application"                // set task group
    val port = 1234
    when {
		// /c instead of /k on some Win11 machines ...
        Os.isFamily(Os.FAMILY_WINDOWS) -> {
            commandLine("cmd", "/k", "for /f \"tokens=5\" %a in ('netstat -aon ^| findstr ${port}') do taskkill /F /PID %a")
        }
        Os.isFamily(Os.FAMILY_MAC) -> {
            commandLine("bash", "-c", "lsof -nti:${port} | xargs kill -9")
        }
        Os.isFamily(Os.FAMILY_UNIX) -> {
            commandLine("bash", "-c", "lsof -nti:${port} | xargs kill -9")
            // commandLine("sh", "-c", "kill \"\$(lsof -t -i:$port)\"") // possible alternative
        }
        else -> { throw GradleException(":cpp:${subProjectName} kill-target -> unknown OS family encountered")}
    }
}

tasks["run"].dependsOn("kill")   // CAREFUL: only suitable for the server side!
tasks["clean"].dependsOn("kill") // CAREFUL: only suitable for the server side!

