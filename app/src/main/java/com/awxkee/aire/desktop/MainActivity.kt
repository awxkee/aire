package com.awxkee.aire.desktop

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Color
import android.graphics.Matrix
import android.os.Bundle
import android.os.FileUtils
import android.util.Log
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
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.tooling.preview.Preview
import androidx.core.graphics.BitmapCompat
import androidx.core.graphics.ColorUtils
import androidx.core.graphics.scale
import com.awxkee.aire.Aire
import com.awxkee.aire.AireColorMapper
import com.awxkee.aire.AirePaletteDithering
import com.awxkee.aire.AireQuantize
import com.awxkee.aire.BitmapScaleMode
import com.awxkee.aire.ColorMatrices
import com.awxkee.aire.desktop.ui.theme.AireDesktopTheme
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.io.ByteArrayOutputStream
import java.io.File
import java.util.UUID
import kotlin.math.PI
import kotlin.system.measureTimeMillis

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
//                        val bitmap =
//                            BitmapFactory.decodeResource(resources, R.drawable.beach_horizon)
//                                .scaleWith(0.5f)
//                        scope.launch {
//                            imagesArray.add(bitmap)
//                        }
//
//                        val kdTime = measureTimeMillis {
//
//                            val compressed = Aire.toJPEG(bitmap, 61)
//                            val bmp = BitmapFactory.decodeByteArray(compressed, 0, compressed.size)
//
//                            val bos = ByteArrayOutputStream()
//                            bitmap.compress(Bitmap.CompressFormat.JPEG, 61, bos)
//                            val bia = bos.toByteArray()
//                            scope.launch {
//                                imagesArray.add(bmp)
//                            }
//
//                            Log.d("AireTest", "Compressed mozjpeg ${compressed.size/1024}kbytes vs regular ${bia.size/1024}")
//                        }
//                        var radius = 5
//                        repeat(25) {
//                            val time = measureTimeMillis {
//                                val glitch = Aire.bokeh(bitmap, radius)
//                                scope.launch {
//                                    imagesArray.add(glitch)
//                                }
//                            }
//                            Log.d("AireMedian", "Radius ${radius} exec time $time")
//                            radius += 3
//                        }
//                        }
//                        Log.d("AireMedian", "exec time $time")
//                        val dilate = Aire.bilateralBlur(bitmap, 5)
//                        scope.launch {
//                            imagesArray.add(dilate)
//                        }
//                        val gray = Aire.grayscale(bitmap)
//                        val tent = Aire.tentBlur(bitmap, 17)
//                        scope.launch {
//                            imagesArray.add(tent)
//                        }
//                        var bmp = Aire.threshold(gray, 37)
//                        scope.launch {
//                            imagesArray.add(bmp)
//                        }
//                        val dilated = Aire.dilate(bmp, 5)
//                        scope.launch {
//                            imagesArray.add(dilated)
//                        }
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

fun Bitmap.scaleTo(width: Int, height: Int) = Bitmap.createScaledBitmap(
    this,
    width.toInt(),
    height.toInt(),
    false
)

fun Bitmap.scaleWith(scale: Float) = Bitmap.createScaledBitmap(
    this,
    (width * scale).toInt(),
    (height * scale).toInt(),
    false
)