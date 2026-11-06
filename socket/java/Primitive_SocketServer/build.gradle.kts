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
    mainClass.set("at.fhooe.sail.vis.socket.Primitive_SocketServer")
}

tasks.named<JavaExec>("run") {
    args("4949") // program arguments
}