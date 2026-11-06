plugins {
    // Apply the application plugin to add support for building a CLI application in Java.
    application
}

repositories {
    // Use Maven Central for resolving dependencies.
    mavenCentral()
}

dependencies {

}

application {
    // Define the main class for the application.
    mainClass.set("at.fhooe.sail.vis.socket.Primitive_SocketClient")
}

tasks.named<JavaExec>("run") {
    args("127.0.0.1", "4949") // program arguments
    standardInput = System.`in` // use commandline input
}