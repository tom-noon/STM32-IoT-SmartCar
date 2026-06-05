
<!-- not use -->

<template>
	<view>
		<uni-card title="查询数据" style="overflow:inherit;">
			<view class="cu-form-group margin-top">
				<view class="title">类型</view>
				<picker @change="cateChange" :value="cateIndex" :range="dataCate">
					<view class="picker">
						{{cateIndex>-1?dataCate[cateIndex]:'请选择需要查询的数据'}}
					</view>
				</picker>
			</view>
			<view class="cu-form-group margin-top">
				<view class="title">查询范围</view>
				<!-- <uni-section :title="'日期时间范围用法：' + '[' + datetimerange + ']' " type="line"></uni-section> -->
				<uni-datetime-picker v-model="datetimerange" type="datetimerange" rangeSeparator="至"/>
			</view>
			<br>
				<button type="primary" @click="getList">查询</button>
				<br>
			<template v-if="dataList.length">
				<scroll-view scroll-y="true">
					<view class="cate" >
						{{dataCate[cateIndex]}}
					</view>
					<table style="width: 100%;" border="1px" cellspacing='0'>
						<tr>
							<th>序号</th>
							<th>数据</th>
							<th>时间</th>
						</tr>
						<tr v-for="(item,index) in showList" style="text-align: center;">
							<td>{{index+1}}</td>
							<td>{{item.data}}</td>
							<td>{{item.created_time}}</td>
						</tr>
					</table>
				</scroll-view><br>
				
				<page-pagination :total="dataList.length" :pageSize="10" :showAround="true" :btnText="true" :showPageInfo="true" @change='change'></page-pagination>
			</template>
			<template v-else>
				<view style="text-align: center;"><h1>暂无数据</h1></view>
			</template>
		</uni-card>

	</view>
</template>

<script>
import request from '@/utils/request.js'
import {toTime,toStamp} from "@/utils/myTime"
import myAlert from '@/utils/myAlert.js'
	export default {
		name: "findData",
		data() {
			return {
				datetimerange: [],
				dataCate:['温度1','温度2','湿度1','湿度2','烟雾1','烟雾2'],
				cateIndex:-1,
				dataList:[],
				showList:[],
				currentPage:1
			};
		},
		methods:{
			cateChange(e) {
				this.cateIndex = e.detail.value
			},
			async getList(){
				
				let islegal=this.vailUrl()
				if(!islegal)return
				let beginT=toStamp(this.datetimerange[0]) 
				let endT=toStamp(this.datetimerange[1])
				await request({url:`${islegal}`}).then(res=>{
					this.dataList=[]
					res.data.data.forEach(item=>{
						let stamp=toStamp(item.created_time)
						if(stamp>beginT&&stamp<endT)
							this.dataList.push(item)
					})
				})
				this.dataList.reverse()
				this.showList=this.dataList.slice(0,10)
			},
		vailUrl(){
			if(this.cateIndex===-1){myAlert('查询类型不能为空');return ''}
			if(this.datetimerange.length<2){myAlert('时间范围不能为空'); return ''}
			let findCate=''
			switch(this.cateIndex){
				case 0:
					findCate='getAllTemp1s'
					break
				case 1:
					findCate='getAllTemp2s'
					break
				case 2:
					findCate='getAllHumid1s'
					break
				case 3:
					findCate='getAllHumid2s'
					break
				case 4:
					findCate='getAllAqi1s'
					break
				case 5:
					findCate='getAllAqi2s'
					break
				default:
					return ''
			}
			return findCate
			
		},
		  change(currentPage, type){
			  this.currentPage = currentPage;
			  if(currentPage)
			  this.showList=this.dataList.slice(currentPage*10-10,currentPage*10)
			  console.log("点击了" + type +"，当前页：" + currentPage);
		}
			
		}
	}
</script>

<style>
.cate{
	font-size: 30px;
	margin: 5px;
	text-align: center;
	font-weight: 700;
}
</style>
