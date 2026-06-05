import Vue from 'vue'
import Vuex from 'vuex'
// import request from '@/utils/request.js'
import {doPublish} from '@/utils/mqtt.js'


Vue.use(Vuex)

export default new Vuex.Store({
	state:{
		time:'',
		// mode:1,
		
		
	},
	mutations:{
	nowTime(state){
		let MyDate=new Date()
		let hour=parseInt(MyDate.getHours())
		let min=parseInt(MyDate.getMinutes())
		let sec=parseInt(MyDate.getSeconds())
		hour<10?hour='0'+hour:''
		min<10?min='0'+min:''
		sec<10?sec='0'+sec:''
		state.time=`${hour}:${min}:${sec}`
	},

},
actions:{

},
	getters:{}
})