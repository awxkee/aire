package com.awxkee.aire.desktop

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
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
import com.awxkee.aire.Aire
import com.awxkee.aire.EdgeMode
import com.awxkee.aire.GaussianPreciseLevel
import com.awxkee.aire.PaletteTransferColorspace
import com.awxkee.aire.ResizeFunction
import com.awxkee.aire.ScaleColorSpace
import com.awxkee.aire.TransferFunction
import com.awxkee.aire.desktop.ui.theme.AireDesktopTheme
import kotlinx.coroutines.asCoroutineDispatcher
import kotlinx.coroutines.launch
import java.util.UUID
import java.util.concurrent.Executors
import kotlin.system.measureTimeMillis

class MainActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            AireDesktopTheme {
                val scope = rememberCoroutineScope()
                val imagesArray = remember {
                    mutableStateListOf<Bitmap>()
                }
                LaunchedEffect(key1 = Unit, block = {
                    scope.launch(Executors.newSingleThreadExecutor().asCoroutineDispatcher()) {
//                        val bitmap =
//                            BitmapFactory.decodeResource(resources, R.drawable.beach_horizon)
//                                .scaleWith(0.7f)
//                                .copy(Bitmap.Config.ARGB_8888, true)

//                        val bitmap1 =
//                            BitmapFactory.decodeResource(resources, R.drawable.haze)
//                                .scaleWith(0.6f)
//                                .copy(Bitmap.Config.ARGB_8888, true)
////////
////                        scope.launch {
////                            imagesArray.add(bitmap)
////                        }
//
////                        delay(2000L)
//
//                        var radius = 77
////
//                        var time = measureTimeMillis {
//                            val image = Aire.gaussianBlur(
//                                bitmap,
//                                75,
//                                0f,
//                                EdgeMode.CLAMP,
//                                GaussianPreciseLevel.INTEGRAL,
//                            )
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        var time2 = measureTimeMillis {
//                            val image = Aire.copyPalette(
//                                bitmap,
//                                bitmap1,
//                                1.0f,
//                                PaletteTransferColorspace.LAB,
//                            )
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        var time3 = measureTimeMillis {
//                            val image = Aire.copyPalette(
//                                bitmap,
//                                bitmap1,
//                                PaletteTransferColorspace.OKLAB,
//                            )
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        var time1 = measureTimeMillis {
//                            val image = Aire.copyPalette(
//                                bitmap,
//                                bitmap1,
//                                PaletteTransferColorspace.LUV,
//                            )
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        Log.d("AireMedian", "Fast blur in ${time}ms")
//                        time = measureTimeMillis {
//                            val image = Aire.claheOklab(
//                                bitmap,
//                                binsCount = 128
//                            )
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        time = measureTimeMillis {
//                            val image = Aire.claheJzazbz(
//                                bitmap,
//                                threshold = 0.2f,
//                                binsCount = 128
//                            )
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        Log.d("AireMedian", "Blur timing linearGaussianBlur done in ${time}ms")
//                        time = measureTimeMillis {
//                            val image = Aire.linearTentBlur(
//                                bitmap,
//                                radius,
//                                TransferFunction.SRGB,
//                            )
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        Log.d("AireMedian", "Blur timing gaussianBoxBlur done in ${time}ms")
//                        time = measureTimeMillis {
//                            val image = Aire.linearGaussianBoxBlur(
//                                bitmap,
//                                radius,
//                                TransferFunction.SRGB,
//                            )
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        Log.d("AireMedian", "Blur timing gaussianBlur done in ${time}ms")
//                        var time = measureTimeMillis {
//                            val image = Aire.scale(
//                                bitmap,
//                                bitmap.width / 4,
//                                bitmap.height / 4,
//                                ResizeFunction.EwaRobidoux,
//                                ScaleColorSpace.SIGMOIDAL
//                            )
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        Log.d("AireMedian", "Blur timing SIGMOIDAL done in ${time}ms")
//                        time = measureTimeMillis {
//                            val image = Aire.scale(
//                                bitmap,
//                                bitmap.width / 4,
//                                bitmap.height / 4,
//                                ResizeFunction.EwaRobidoux,
//                                ScaleColorSpace.LUV
//                            )
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        Log.d("AireMedian", "Blur timing LUV done in ${time}ms")
//                        time = measureTimeMillis {
//                            val image = Aire.scale(
//                                bitmap,
//                                bitmap.width / 4,
//                                bitmap.height / 4,
//                                ResizeFunction.EwaRobidoux,
//                                ScaleColorSpace.XYZ
//                            )
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        Log.d("AireMedian", "Blur timing LUV done in ${time}ms")
//                        time = measureTimeMillis {
//                            val image = Aire.scale(
//                                bitmap,
//                                bitmap.width / 4,
//                                bitmap.height / 4,
//                                ResizeFunction.EwaRobidoux,
//                                ScaleColorSpace.LINEAR
//                            )
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        Log.d("AireMedian", "Blur timing LUV done in ${time}ms")
//
//                        time = measureTimeMillis {
//                            val image = Aire.stackBlur(bitmap, radius)
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        Log.d("AireMedian", "Blur timing stackBlur done in ${time}ms")
//
//                        time = measureTimeMillis {
//                            val image = Aire.tentBlur(bitmap, radius)
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        Log.d("AireMedian", "Blur timing tentBlur done in ${time}ms")
//
//                        time = measureTimeMillis {
//                            val image = Aire.fastGaussian2Degree(bitmap, radius)
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        Log.d("AireMedian", "Blur timing fastGaussian2Degree done in ${time}ms")
//
//                        time = measureTimeMillis {
//                            val image = Aire.boxBlur(bitmap, radius)
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        Log.d("AireMedian", "Blur timing boxBlur done in ${time}ms")


//                        val entries = ResizeFunction.entries
//                        for (entry in entries) {
//                            val scaled: Bitmap
//                            Log.d("AireMedian", "Started $entry")
//                            val time = measureTimeMillis {
//                                scaled = Aire.scale(
//                                    bitmap,
//                                    bitmap.width / 6,
//                                    bitmap.height / 6,
//                                    entry,
//                                    ScaleColorSpace.SRGB
//                                )
//                            }
//                            Log.d("AireMedian", "$entry done in ${time}ms")
//
//                            scope.launch {
//                                imagesArray.add(
//                                    scaled
//                                )
//                            }
//                        }

//                        val eq = Aire.equalizeHistAdaptive(bitmap, 6, 6)
//                        scope.launch {
//                            imagesArray.add(eq)
//                        }
//
//                        val eqh = Aire.equalizeHistAdaptiveHSV(bitmap, 6, 6,128)
//                        scope.launch {
//                            imagesArray.add(eqh)
//                        }
//
//                        val eqhv = Aire.equalizeHistAdaptiveHSL(bitmap,6, 6,128)
//                        scope.launch {
//                            imagesArray.add(eqhv)
//                        }
//
//                        val eqhsl = Aire.equalizeHistAdaptiveLAB(bitmap, 6, 6,128)
//                        scope.launch {
//                            imagesArray.add(eqhsl)
//                        }
//
//                        val eqhsv = Aire.equalizeHistAdaptiveLUV(bitmap, 6, 6,128)
//                        scope.launch {
//                            imagesArray.add(eqhsv)
//                        }

//                        val clahe = Aire.clahe(bitmap, 6f, 6, 6)
//                        scope.launch {
//                            imagesArray.add(clahe)
//                        }
//
//                        val claheLAB = Aire.claheLAB(bitmap, 2f, 8, 8, 128)
//                        scope.launch {
//                            imagesArray.add(claheLAB)
//                        }
//
//                        val claheLUV = Aire.claheLUV(bitmap, 2f, 8, 8, 128)
//                        scope.launch {
//                            imagesArray.add(claheLUV)
//                        }
//
//                        val hh = Aire.equalizeHistAdaptiveLAB(bitmap, 8, 8, 128)
//                        scope.launch {
//                            imagesArray.add(hh)
//                        }

//                        val hluv = Aire.equalizeHistAdaptiveLUV(bitmap)
//                        scope.launch {
//                            imagesArray.add(hluv)
//                        }
//                        val sqr = Aire.equalizeHistSquares(bitmap)
//                        scope.launch {
//                            imagesArray.add(sqr)
//                        }

//                        val values = BitmapScaleMode.entries.toTypedArray()
//                        values.forEach { mode ->
//                            val compressedJpegsRGB = Aire.scale(
//                                bitmap,
//                                bitmap.width / 3, bitmap.height / 3,
//                                mode, ScaleColorSpace.SRGB
//                            )
//
//                            scope.launch {
//                                imagesArray.add(compressedJpegsRGB)
//                            }
//
//                            val compressedJpegsLAB = Aire.scale(bitmap,
//                                bitmap.width / 3, bitmap.height / 3,
//                                mode, ScaleColorSpace.LAB)
//
//                            scope.launch {
//                                imagesArray.add(compressedJpegsLAB)
//                            }
////
////                            val compressedJpegsLUV = Aire.scale(bitmap,
////                                bitmap.width / 5, bitmap.height / 5,
////                                mode, ScaleColorSpace.LUV)
////
////                            scope.launch {
////                                imagesArray.add(compressedJpegsLUV)
////                            }
//
//                            val compressedJpegsLinear = Aire.scale(bitmap,
//                                bitmap.width / 3, bitmap.height / 3,
//                                mode, ScaleColorSpace.LINEAR)
//
//                            scope.launch {
//                                imagesArray.add(compressedJpegsLinear)
//                            }
//                        }
//
//                        repeat(1) {
//                            val d2Time = measureTimeMillis {
//                                val compressedJpeg = Aire.equalizeHist(bitmap)
//
//                                scope.launch {
//                                    imagesArray.add(compressedJpeg)
//                                }
//                            }
//                            executionsFast.add(d2Time)
//                        }
//                        Log.d("AireMedian", "Fast gaussian exec time ${executionsFast.average()}")
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