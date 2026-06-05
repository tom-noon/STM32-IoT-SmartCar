import mqtt from 'mqtt/dist/mqtt.js'
import store from '../store'
import {MQTTURL} from './env'
// 连接配置
const connection = {
	host: MQTTURL,
	port: 8083,
	endpoint: '/mqtt',
	clean: true,
	// keepalive: 60,
	connectTimeout: 1800000,
	reconnectPeriod: 2000,
	clientId: '',
	username: 'SC_control',
	password: '123456'
}

var client = null
// 建立连接
export function createConnection() {
	const {
		host,
		port,
		endpoint,
		...options
	} = connection
	// #ifdef H5 
	options.clientId = 'ts_H5'
	const connectUrl = `ws://${host}:${port}${endpoint}`
	// #endif
	// #ifdef MP-WEIXIN||APP-PLUS
	options.clientId = 'ts_APP'
	const connectUrl = `wx://${host}:${port}${endpoint}`
	// #endif
	try {
		client = mqtt.connect(connectUrl, options)
	} catch (err) {
		console.log('mqtt.connect error', err)
	}
	// 监听连接成功
	client.on('connect', () => {
		// store.commit('CHANGEONLINE', 1)
		console.log('连接成功')
	})
	// 监听连接失败 
	client.on('error', err => {
		console.log('连接失败', err)
	})
	// 监听消息传输
	client.on('message', (topic, message) => {
		// console.log(message);
		message = JSON.parse(message.toString())
		
		// console.log(`收到消息 ${message} 来自 ${topic}`)
		
		
		if (topic === "testtopic/data")
		{
			store.commit('mode/CHANGESTATE', message)
			
			store.commit("temp/RECEIVETEMP", message)
			
			store.commit("humi/RECEIVEHUMI", message)
			
			store.commit("fan/RECEIVESPD", message)
		}
		
	})
	// 监听断开连接
	client.on('close', () => {
		console.log('连接关闭')
	})
	// 监听重连
	// if(!store.state.isOnline)
	client.on('reconnect', () => {
		console.log('正在重连');
	})
}

// 订阅主题
export function doSubscribe(topic) {
	const qos = 1
	client.subscribe(topic, {
		qos
	}, (err, res) => {
		if (err) {
			console.log('Subscribe to topics error', err)
			return
		}
		console.log('订阅的主题', res)
	})
}
// 发布主题和消息
export function doPublish(topic, payload) {
	const qos = 2
	client.publish(topic, payload,{qos,retain:true}, err => {
		if (err) {
			console.log('Publish error', err)
		} else {
			console.log('发送成功',qos, JSON.parse(payload));
		}
	})
}

export function unSub(topic) {
	client.unsubscribe(topic, function(err) {
		if (err) {
			console.log(err);
		} else {
			console.log(`取消订阅,主题${topic}`);
		}
	})
}
