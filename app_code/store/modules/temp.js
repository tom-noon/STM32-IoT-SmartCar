// import request from '@/utils/request.js'
import {toStamp,toTime} from '@/utils/myTime.js'

const state={
	temp1:null,
	// temp2:null,
	temp1List:{createT:[],data:[]},
	// temp2List:{createT:[],data:[]},
	biggerTemp1:null,
	// biggerTemp2:null,
	lessTemp1:null,
// 	lessTemp2:null,
};
const moment=require('moment')
const mutations={
	// 接收硬件发来的温度数据
	RECEIVETEMP(state,value){
		// console.log(value);
		this.commit("temp/UPDATATEMP",value)
		
		let stamp=Math.floor(new Date().getTime())
		let time=moment(stamp).format('MM/DD HH:mm:ss')
		
		state.temp1=value.temp1
		if(state.temp1List.data.length>9){
			state.temp1List.data.shift()
			state.temp1List.createT.shift()
			state.temp1List.data.push(value.temp1)
			state.temp1List.createT.push(time)
		}
		else
		{
			state.temp1List.data.push(value.temp1)
			state.temp1List.createT.push(time)
		}
		uni.setStorageSync('temp1List',state.temp1List)
		
	},
	UPDATATEMP(state,value){
		if(value.temp1>25)state.biggerTemp1++
		else state.lessTemp1--
	},

	
	// 初始化统计温度s
	temp1Tj(state,value){
		if(!value.length)return
			state.temp1=value[value.length-1].data
			state.biggerTemp1=value.filter((item)=>item.data>25).length
			state.lessTemp1=value.filter((item)=>item.data<25).length
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