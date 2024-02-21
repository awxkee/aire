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
                        val bitmap =
                            BitmapFactory.decodeResource(resources, R.drawable.haze)
                                .scaleWith(0.8f)
                        scope.launch {
                            imagesArray.add(bitmap)
                        }
//
                        repeat(8) {
                            val d2Time = measureTimeMillis {
                                val blurred3 = Aire.fastGaussian2Degree(bitmap, 15)
                                scope.launch {
                                    imagesArray.add(blurred3)
                                }
                            }
                            val d3Time = measureTimeMillis {
                                val blurred3 = Aire.fastGaussian3Degree(bitmap, 15)
                                scope.launch {
                                    imagesArray.add(blurred3)
                                }
                            }
                            val d4Time = measureTimeMillis {
                                val blurred = Aire.fastGaussian4Degree(bitmap, 15)
                                scope.launch {
                                    imagesArray.add(blurred)
                                }
                            }
                            val stack = measureTimeMillis {
                                val blurred3 = Aire.stackBlur(bitmap, 15)
                                scope.launch {
                                    imagesArray.add(blurred3)
                                }
                            }
                            val gauss = measureTimeMillis {
                                val blurred3 = Aire.gaussianBlur(bitmap, 15, 15f)
                                scope.launch {
                                    imagesArray.add(blurred3)
                                }
                            }
                            Log.d("AireMedian", "Done 2D blur in ${d2Time}ms, 3D Blur in ${d3Time}ms, 4D Blur in ${d4Time}ms stack blur in ${stack}ms, gauss ${gauss}ms")
                        }
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