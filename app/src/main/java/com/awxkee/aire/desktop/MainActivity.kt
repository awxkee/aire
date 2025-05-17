package com.awxkee.aire.desktop

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Slider
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
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
import com.awxkee.aire.AireQuantize
import com.awxkee.aire.ConvolveKernels
import com.awxkee.aire.EdgeMode
import com.awxkee.aire.GaussianPreciseLevel
import com.awxkee.aire.KernelShape
import com.awxkee.aire.MorphKernels
import com.awxkee.aire.MorphOp
import com.awxkee.aire.MorphOpMode
import com.awxkee.aire.PaletteTransferColorspace
import com.awxkee.aire.ResizeFunction
import com.awxkee.aire.Scalar
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
//        val bitmap0 =
//            BitmapFactory.decodeResource(resources, R.drawable.test_augea)
//                .copy(Bitmap.Config.ARGB_8888, true)
        setContent {
            AireDesktopTheme {
                val scope = rememberCoroutineScope()
                val imagesArray = remember {
                    mutableStateListOf<Bitmap>()
                }
                LaunchedEffect(key1 = Unit, block = {
                    scope.launch(Executors.newSingleThreadExecutor().asCoroutineDispatcher()) {
//                        val bitmap =
//                            BitmapFactory.decodeResource(resources, R.drawable.haze)
//                                .copy(Bitmap.Config.ARGB_8888, true)
//                                .scaleWith(0.4f)
//
////                        val bitmap1 =
////                            BitmapFactory.decodeResource(resources, R.drawable.haze)
////                                .scaleWith(0.6f)
////                                .copy(Bitmap.Config.ARGB_8888, true)
//////
////                        scope.launch {
////                            imagesArray.add(bitmap)
////                        }
//
//////                        delay(2000L)
////
////                        var radius = 77
//////
//                        var time = measureTimeMillis {
//                            val image = Aire.boxBlur(bitmap, 6)
//                            scope.launch {
//                                imagesArray.add(image)
//                            }
//                        }
//                        Log.d("MainActivity", "Exec time $time")
                    }
                })
//                var sliderValue by remember { mutableFloatStateOf(100f) }  // Initial value of the slider
//
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    Column(modifier = Modifier.fillMaxWidth()) {
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
//
//                        Slider(
//                            value = sliderValue,
//                            onValueChange = { newValue ->
//
//                                val image = Aire.saturation(bitmap = bitmap0, newValue / 100f, true)
//                                scope.launch {
//                                    imagesArray.clear()
//                                    imagesArray.add(image)
//                                }
//
//                                sliderValue = newValue
//                            },
//                            valueRange = 0f..1000f  // Defines the min and max range of the slider
//                        )
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