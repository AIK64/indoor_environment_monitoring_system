import 'dart:async';
import 'dart:convert';
import 'dart:ui';
import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:dio/dio.dart';
import 'package:flutter/widgets.dart';


//对DIO进行初始化
Dio dio=Dio(
    BaseOptions(
      //baseUrl: 'https://api.pub.dev'
        connectTimeout: Duration(seconds: 3),
        receiveTimeout: Duration(seconds: 3),
        headers: {
          "authorization":"version=2018-10-31&res=products%2Fa32kUt5E2v%2Fdevices%2FIEMSHardware&et=1871122235&method=md5&sign=VmZJzEEg5%2BQLemcCxreJIg%3D%3D"
        }
    )
);


class DisplayData{
  DisplayData(this.dataPointName,this.updatedTime,this.value,this.label,this.icon,this.unit);

  String dataPointName;
  String updatedTime;
  dynamic value;
  String label;
  Widget icon;
  String unit;

}


void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        // This is the theme of your application.
        //
        // TRY THIS: Try running your application with "flutter run". You'll see
        // the application has a purple toolbar. Then, without quitting the app,
        // try changing the seedColor in the colorScheme below to Colors.green
        // and then invoke "hot reload" (save your changes or press the "hot
        // reload" button in a Flutter-supported IDE, or press "r" if you used
        // the command line to start the app).
        //
        // Notice that the counter didn't reset back to zero; the application
        // state is not lost during the reload. To reset the state, use hot
        // restart instead.
        //
        // This works for code too, not just values: Most code changes can be
        // tested with just a hot reload.
          colorScheme: ColorScheme.fromSeed(seedColor: Colors.blueAccent),
          useMaterial3: true,
          fontFamily: 'Roboto'
      ),
      home: const MyHomePage(title: '室内环境监测系统数据'),
    );
  }
}

//首页类的实现
class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  // This widget is the home page of your application. It is stateful, meaning
  // that it has a State object (defined below) that contains fields that affect
  // how it looks.

  // This class is the configuration for the state. It holds the values (in this
  // case the title) provided by the parent (in this case the App widget) and
  // used by the build method of the State. Fields in a Widget subclass are
  // always marked "final".

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  int currentPageIndex = 0;
  double temperature=26;
  int humidity=56;
  String selectedPage='数据流';
  int autoUpdateTime=2;
  late Timer _timer;

  String nowtime='';

  TextEditingController autoUpdateTimeController=TextEditingController();

  List<DisplayData> displayDataList=<DisplayData>[
    DisplayData('temperature','1999-04-14 20:10:31',0,'温度',Icon(Icons.thermostat),'°C'),
    DisplayData('humidity','1999-04-14 20:10:31',0,'湿度',Icon(Icons.water_drop),'%'),
    //DisplayData('LightIntensity','1999-04-14 20:10:31',0,'光强',Icon(Icons.light_mode),'lux'),
    DisplayData('CH2O','1999-04-14 20:10:31',0,'甲醛',Icon(Icons.energy_savings_leaf),'mg/m3')
  ];

  Map<String,DisplayData> displayDataMap={};


  //进行参数dataValList,dataPointMap等的初始化
  void initDisplayDataMap()
  {//初始化map
    displayDataList.forEach((data) {
      String dataPointName=data.dataPointName;
      displayDataMap[dataPointName]=data;
    });
  }

  void initTimer()
  {

    _timer=Timer.periodic(Duration(seconds: autoUpdateTime), (Timer timer) {
      _RefreshData();
    });

  }


  void _RefreshData() async {
    Response response;
    response=await dio.get("https://iot-api.heclouds.com/datapoint/current-datapoints",
        queryParameters: { "product_id":"a32kUt5E2v","device_name":"IEMSHardware" }
    );

    Map<String, dynamic> jsonData = json.decode(response.toString());
    //print(jsonData);
    List<dynamic> datastreams=jsonData['data']['devices'];
    Map<String, dynamic> datastreamsMap=datastreams[0];
    List<dynamic> dataStreamList=datastreamsMap['datastreams'];
    for(var item in dataStreamList)
    {
      if(displayDataMap.containsKey(item['id']))
      {
        displayDataMap[item['id']]?.value=item['value'];
        displayDataMap[item['id']]?.updatedTime=item['at'];
      }
    }

    nowtime=DateTime.now().toString();

    setState(() {});
  }

  @override
  void initState(){
    super.initState();
    initDisplayDataMap();
    initTimer();
    _RefreshData();

  }


  @override
  Widget build(BuildContext context) {
    final ThemeData theme = Theme.of(context);
    // This method is rerun every time setState is called, for instance as done
    // by the _incrementCounter method above.
    //
    // The Flutter framework has been optimized to make rerunning build methods
    // fast, so that you can just rebuild anything that needs updating rather
    // than having to individually change instances of widgets.
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
        title: Text(selectedPage),
      ),
      body: <Widget>[

        //设备页面
        ListView.builder(
            itemCount: displayDataList.length,
            itemExtent: 100,
            itemBuilder: (BuildContext context,int index){
              return  Container(
                margin: EdgeInsets.all(1.0),
                alignment: Alignment.center,
                child: Card(
                    child: InkWell(
                      splashColor: Colors.blueAccent.withAlpha(15),
                      onTap: (){
                        Navigator.push(context, MaterialPageRoute(builder: (context){
                          return dataDetailPage(
                            dataPointName:displayDataList[index].dataPointName,
                            label: displayDataList[index].label,
                            value: displayDataList[index].value,
                            icon: displayDataList[index].icon,
                          );
                        }));
                      },
                      child: Row(
                        //mainAxisAlignment: MainAxisAlignment.center,
                        //mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                        children: <Widget>[
                          Container(
                            //padding是把空白填充在内部，margin则是填充在container的外部
                            padding: EdgeInsets.all(30.0),
                            child: displayDataList[index].icon,
                          ),
                          Column(
                            mainAxisAlignment: MainAxisAlignment.center,
                            children: [
                              Text('${displayDataList[index].label}:${displayDataList[index].value}${displayDataList[index].unit}',style: TextStyle(fontSize: 25),),
                              Text('采集时间：${displayDataList[index].updatedTime}',style: TextStyle(color: Colors.grey,fontSize: 10),),
                            ],
                          )

                        ],
                      ),
                    )
                ),
              );
            }),

        //设置页面
        ListView(
          children: <Widget>[
            ListTile(
              minVerticalPadding: 25,
              leading: Icon(Icons.settings,size: 30,),
              title: Text("设置自动更新周期（当前：${autoUpdateTime}s）",style: TextStyle(fontSize: 20),),
              onTap: () async {
                bool? isok = await  showAutoUpdatedTimeDialog(context,autoUpdateTimeController);
                if(isok!=null)
                {
                  if(isok)
                  {//说明进行了提交更改
                    int revNum=int.parse(autoUpdateTimeController.text);
                    if(revNum<0)
                    {
                      autoUpdateTimeController.text=autoUpdateTime.toString();
                    }
                    else if(revNum==0)
                    {
                      _timer.cancel();
                    }
                    else
                    {
                      _timer.cancel();
                      autoUpdateTime=revNum;
                      initTimer();
                    }
                  }
                }
                setState(() {

                });
              },
            ),
            ListTile(
              title: Text('刷新时间:$nowtime'),
            )
          ],
        ),


        //关于页面
        ListView(
          padding: EdgeInsets.all(8),
          children:<Widget> [
            // ListTile(
            //   leading: Icon(Icons.person_outline),
            //   title: Text("作者：540" ,style: TextStyle(fontSize: 20),),
            // ),
            ListTile(
              leading: Icon(Icons.file_copy),
              title: Text('版本v0.1',style: TextStyle(fontSize: 20),),
            )
          ],
        )
      ][currentPageIndex],

      drawer: Drawer(
        child: ListView(
          padding: EdgeInsets.zero,
          children: <Widget>[
            const DrawerHeader(
              decoration: BoxDecoration(
                color: Colors.blueAccent,
              ),
              child: Text(
                '室内环境监测系统数据',
                style: TextStyle(
                  color: Colors.white,
                  fontSize: 24,
                ),
              ),
            ),
            ListTile(
              leading: const Icon(Icons.devices),
              title: const Text('数据流'),
              onTap: () {
                setState(() {
                  selectedPage = '数据流';
                  currentPageIndex=0;
                });
                Navigator.pop(context);
              },
            ),
            ListTile(
              leading: const Icon(Icons.settings),
              title: const Text('设置'),
              onTap: () {
                setState(() {
                  selectedPage = '设置';
                  currentPageIndex=1;
                });
                Navigator.pop(context);
              },
            ),
            ListTile(
              leading: const Icon(Icons.person_outline),
              title: const Text('关于'),
              onTap: () {
                setState(() {
                  selectedPage = '关于';
                  currentPageIndex=2;
                });
                Navigator.pop(context);
              },
            ),
          ],
        ),
      ),
      floatingActionButton:currentPageIndex==0
      ?FloatingActionButton(
        onPressed: _RefreshData,
        tooltip: 'Increment',
        child: const Icon(Icons.refresh),
      ):null // This trailing comma makes auto-formatting nicer for build methods.
    );
  }
}

Future<bool?> showAutoUpdatedTimeDialog(BuildContext context,TextEditingController controller) {
  return showDialog<bool>(
      context: context,
      builder: (context){
        return AlertDialog(
          title:  Text('设置自动更新周期'),
          content: TextField(
            autofocus: true,
            controller: controller,
            keyboardType: TextInputType.number,
            decoration: InputDecoration(
                border: OutlineInputBorder(),
                labelText: '单位：秒'
            ),
            onSubmitted: (value){
              Navigator.of(context).pop(true);
            },
          ),
          actions:<Widget>[
            TextButton(
                onPressed: (){
                  Navigator.of(context).pop(true);
                },
                child: Text('确定')),
            TextButton(
                onPressed: (){
                  Navigator.of(context).pop(false);
                },
                child: Text('取消')),
          ],
        );
      });
}

class dataDetailPage extends StatefulWidget {
  dataDetailPage({
    Key? key,
    required this.dataPointName,
    required this.label,
    required this.value,
    required this.icon,

  }): super(key: key);

  String dataPointName;
  String label;
  dynamic value;
  Widget icon;

  @override
  _dataDetailPageState createState() => _dataDetailPageState();
}

class _dataDetailPageState extends State<dataDetailPage>{
  List<FlSpot> spotList=[
    FlSpot(1715419225367, 40),
    FlSpot(1715419225167, 40),
    FlSpot(1715419221367, 40),
  ];

  List<FlSpot> tempList=[
    FlSpot(1715419225367, 40),
    FlSpot(1715419225167, 40),
    FlSpot(1715419221367, 40),
  ];



  Future<void> getHistoryData() async
  {
    Duration duration= Duration(days: 10);
    DateTime startDate=DateTime.now().subtract(duration);
    DateTime endDate=DateTime.now();
    Response response;
    //获取的是当前一天之间的时间
    response=await dio.get("https://iot-api.heclouds.com/datapoint/history-datapoints",
        queryParameters: { "product_id":"a32kUt5E2v","device_name":"IEMSHardware",'datastream_id':widget.dataPointName,
          'start':startDate.toIso8601String(),'end':endDate.toIso8601String(),'limit':'10'}
    );

    Map<String, dynamic> jsonData = json.decode(response.toString());
    //print(jsonData);
    List<dynamic> datastreams=jsonData['data']['datastreams'];
    if(datastreams.isEmpty)
    {
      return;
    }
    Map<String, dynamic> datastreamsMap=datastreams[0];
    List<dynamic> datapointsList=datastreamsMap['datapoints'];

    List<FlSpot> updatedSpotList = [];
    for(var item in datapointsList)
    {
      String at=item['at'];
      double datetime=DateTime.parse(at).millisecondsSinceEpoch.toDouble();
      updatedSpotList.add(FlSpot(datetime, item['value']));
    }
    setState(() {
      spotList=updatedSpotList;
    });
    //print(spotList);
  }

  @override
  void initState()
  {
    super.initState();
    getHistoryData();
    print(spotList);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("历史数据"),
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
      ),
      body: Center(
        child: Container(

          child: ListView(
            children: [
              ListTile(
                title: Card(
                  margin: EdgeInsets.only(bottom: 5),
                  child: Row(
                    children: [
                      Container(
                        padding: EdgeInsets.all(25),
                        child: widget.icon,
                      ),
                      Container(
                        child: Text("${widget.label}:${widget.value}",textAlign: TextAlign.center,style: TextStyle(fontSize: 25),),
                      )
                    ],
                  ),
                ),
              ),
              ListTile(
                  title:Container(
                    //color: Colors.grey,
                    // padding: EdgeInsets.only(top: 1),
                    height: 275,
                    child:Card(
                      // margin: EdgeInsets.all(10),
                        child: Stack(children: [
                          SizedBox(
                            width: 60,
                            height: 34,
                            child:Text("${widget.label}"),
                          ),
                          Container(
                            padding: EdgeInsets.all(10),
                            child: LineChart(
                              mainData(),
                            ),
                          )
                        ],)
                    ),
                  )
              ),
            ],
          ),
        ),
      ),
    );
  }

  SideTitleWidget bottomTitleWidgets(double value,TitleMeta meta)
  {
    const style = TextStyle(
      fontWeight: FontWeight.bold,
      fontSize: 12,
    );
    Widget text;
    DateTime dateTime=DateTime.fromMillisecondsSinceEpoch(value.toInt());
    print(value);
    //print(dateTime);
    String date=dateTime.toIso8601String().split("T")[1];
    date=date.split(".")[0];
    text=Text(date,style:style,);
    return SideTitleWidget(child: text, axisSide: meta.axisSide);
  }

  Widget leftTitleWidgets(double value,TitleMeta meta)
  {
    //print(value);
    const style=TextStyle(
      fontWeight: FontWeight.bold,
      fontSize: 15,
    );
    String text;
    switch (value.toInt()) {
      case 10:
        text = '10';
        break;
      case 30:
        text = '30';
        break;
      case 50:
        text = '50';
        break;
      case 70:
        text = '70';
        break;
      case 90:
        text = '90';
        break;
      default:
        return Container();
    }
    //print("value in left:");
    //print(value);
    return Text(text,style: style,textAlign: TextAlign.left,);
  }


  LineChartData mainData(){
    return LineChartData(
      gridData: const FlGridData(
        show: true,
        drawVerticalLine: true,
        drawHorizontalLine: true,
      ),
      titlesData: FlTitlesData(
        show: true,
        bottomTitles: AxisTitles(
            sideTitles: SideTitles(
                showTitles: true,
                //标题宽度
                reservedSize: 30,
                interval: 100000,
                getTitlesWidget: bottomTitleWidgets
            )
        ),
        leftTitles: AxisTitles(
            sideTitles: SideTitles(
              showTitles: true,
              interval: 1,
              reservedSize: 42,
              getTitlesWidget: leftTitleWidgets,
            )
        ),
        rightTitles: const AxisTitles(
            sideTitles: SideTitles(
              showTitles: false,
            )
        ),
        topTitles: const AxisTitles(
            sideTitles: SideTitles(
              showTitles: false,
            )
        ),
      ),
      borderData: FlBorderData(
        show: true,
        border: Border.all(color: Color(0xff138af1)),
      ),
      minX: spotList.last.x,
      maxX: spotList.first.x,
      minY: 0,
      maxY: 100,
      lineBarsData: [
        LineChartBarData(
          //定义了数据源list
          spots: spotList,
          isCurved: true,
          //曲线颜色
          color: const Color.fromRGBO(19, 138, 241, 1.0),
          barWidth: 2,
          isStrokeCapRound: true,
          dotData: FlDotData(
            show: true,
          ),
        )
      ],
    );
  }

}