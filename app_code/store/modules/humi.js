import {toStamp,toTime} from '@/utils/myTime.js'
// import request from '@/utils/request.js'
const state={
	humi1:null,
	humi1List:{createT:[],data:[]},
};
const moment=require('moment')
const mutations={
	
	// 接收硬件发来的湿度数据
	RECEIVEHUMI(state,value){
		let stamp=Math.floor(new Date().getTime())
		let time=moment(stamp).format('MM/DD HH:mm:ss')
		
		state.humi1=value.humi1
		if(state.humi1List.data.length>9){
			state.humi1List.data.shift()
			state.humi1List.createT.shift()
			state.humi1List.data.push(value.humi1)
			state.humi1List.createT.push(time)
		}
		else
		{
			state.humi1List.data.push(value.humi1)
			state.humi1List.createT.push(time)
		}
		
		uni.setStorageSync("humi1List",state.humi1List)
	},
};
const actions={
};
const getters={
	
}
export default {
  namespaced: true,
  state,
  getters,
  actions,
  mutations,
};