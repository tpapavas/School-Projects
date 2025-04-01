import java.net.URI

plugins {
    alias(libs.plugins.androidApplication)
    id("com.google.gms.google-services")
}

android {
    namespace = "com.example.insidethescore"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.example.insidethescore"
        minSdk = 26
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
}

dependencies {

    implementation(libs.appcompat)
    implementation(libs.material)
    implementation(libs.activity)
    implementation(libs.constraintlayout)
    implementation("androidx.recyclerview:recyclerview:1.3.2")
    implementation("androidx.cardview:cardview:1.0.0")
    implementation("androidx.window:window:1.3.0")
    implementation("androidx.transition:transition:1.5.0")
    implementation("com.google.android.material:material:1.12.0")
    implementation("androidx.gridlayout:gridlayout:1.0.0")
    implementation(libs.firebase.database)
    implementation(libs.firebase.firestore)
    implementation("com.firebaseui:firebase-ui-storage:7.2.0")
    implementation(libs.firebase.storage)
    implementation("com.github.bumptech.glide:glide:4.14.2")
    implementation("org.apache.commons:commons-text:1.12.0")
    testImplementation(libs.junit)
    androidTestImplementation(libs.ext.junit)
    androidTestImplementation(libs.espresso.core)
    annotationProcessor("com.github.bumptech.glide:compiler:4.14.2")
}