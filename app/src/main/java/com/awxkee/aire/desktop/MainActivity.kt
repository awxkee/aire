package com.awxkee.aire.desktop

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.tooling.preview.Preview
import com.awxkee.aire.Aire
import com.awxkee.aire.desktop.ui.theme.AireDesktopTheme
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.util.UUID

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            AireDesktopTheme {
                val scope = rememberCoroutineScope()
                var imagesArray = remember {
                    mutableStateListOf<Bitmap>()
                }
                LaunchedEffect(key1 = Unit, block = {

                    scope.launch(Dispatchers.IO) {
                        val bitmap = BitmapFactory.decodeResource(resources, R.drawable.cyberpunk).scaleWith(0.15f)
                        scope.launch {
                            imagesArray.add(bitmap)
                        }
                        val bmp = Aire.glitch(bitmap, -0.0075f, -0.0075f)
//                        val bmp = Aire.gaussianBlur(bitmap, 30, 7f)
                        scope.launch {
                            imagesArray.add(bmp)
                        }
                    }
                })
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    LazyColumn(
                        modifier = Modifier
                            .fillMaxWidth()
                    ) {
                        items(imagesArray.count(), key = {
                            return@items UUID.randomUUID().toString()
                        }) {
                            Image(
                                bitmap = imagesArray[it].asImageBitmap(),
                                modifier = Modifier.fillMaxWidth(),
                                contentScale = ContentScale.FillWidth,
                                contentDescription = "ok"
                            )
                        }
                    }
                }
            }
        }
    }
}

@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {
    Text(
        text = "Hello $name!",
        modifier = modifier
    )
}

@Preview(showBackground = true)
@Composable
fun GreetingPreview() {
    AireDesktopTheme {
        Greeting("Android")
    }
}

fun Bitmap.scaleWith(scale: Float) = Bitmap.createScaledBitmap(
    this,
    (width * scale).toInt(),
    (height * scale).toInt(),
    false
)