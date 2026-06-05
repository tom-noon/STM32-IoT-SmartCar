import {
	doPublish
} from '@/utils/mqtt'

let id=2024

export function mode(i=0){
	let modeInfo={"id":id,"mode":i}
	doPublish("testtopic/cmd",JSON.stringify(modeInfo))
}

export function run(i=0,j=0,k=0) {
	let runInfo={"id":id,"dir":i,"speed":j,"runtime":k}
	doPublish("testtopic/cmd",JSON.stringify(runInfo))
}

export function led(i=0) {
	let ledInfo={"id":id,"led0":i}
	doPublish("testtopic/cmd",JSON.stringify(ledInfo))
}


// // 控制led灯
// export function led(i=0,j=0) {
// 	let ledInfo={"led1":i,"led2":j}
// 	doPublish('testtopic/mode',JSON.stringify(ledInfo))
// }
// // 控制风扇
// export function fan(i=0,j=0){
// 	let fanInfo={"fan1":i,"fan2":j}
// 	doPublish("testtopic/mode",JSON.stringify(fanInfo))
// }
// // 修改工作模式
// export function mode(i=1){
// 	let modeInfo={"mode":i}
// 	doPublish("testtopic/mode",JSON.stringify(modeInfo))
// }
// // 控制蜂鸣器
// export function buzzer(i=0){
// 	let buzzer={"buzzer":i}
// 	doPublish("testtopic/mode",JSON.stringify(buzzer))
// }
// // 发送人数
// export function people(i=0){
// 	let people={"people":i}
// 	doPublish('testtopic/mode',JSON.stringify(people))
// }
// // 控制步进电机
// export function motor(i=3){
// 	let motor={"motor":i}
// 	doPublish("testtopic/mode",JSON.stringify(motor))
// }
// // 重启自动发送时间
// export function time(){
// 	let time={"time":Math.floor(new Date().getTime()/1000)}
// 	doPublish("testtopic/mode",JSON.stringify(time))
// }
// // 温度阈值
// export function temp(i){
// 	let temp={"temp":i}
// 	doPublish('testtopic/mode',JSON.stringify(temp))
// }
// // 湿度阈值
// export function humi(i){
// 	let humi={"humi":i}
// 	doPublish('testtopic/mode',JSON.stringify(humi))
// }
// // 空气阈值
// export function air(i){
// 	let air={"air":i}
// 	doPublish('testtopic/mode',JSON.stringify(air))
// }
// // 光照阈值
// export function light(i){
// 	let light={"light":i}
// 	doPublish('testtopic/mode',JSON.stringify(light))
// }

// // 开锁
// export function lock(i=1){
// 	let lock={"lock":i}
// 	doPublish('testtopic/mode',JSON.stringify(lock))
// }
